// yuvの画像サイズを変更する動作。拡大縮小します。
// なお切り抜きしたい場合は、dataPosとwidth height値をいじればそれでOK
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>

#ifdef __ENABLE__
#   include <ttLibC/resampler/imageResizer.h>
#endif

#include <ttLibC/frame/video/yuv420.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class YuvImageResizer : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        ttLibC_Allocator_init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("YuvImageResizer").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "resample", Resample);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("YuvImageResizer").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit YuvImageResizer(
            const char *yuvType,
            uint32_t    width,
            uint32_t    height,
            bool        isQuick) : 
                width_(width), height_(height), isQuick_(isQuick) {
        if(strcmp(yuvType, "planar") == 0) {
            yuvType_ = Yuv420Type_planar;
        }
        else if(strcmp(yuvType, "semiPlanar") == 0) {
            yuvType_ = Yuv420Type_semiPlanar;
        }
        else if(strcmp(yuvType, "planarYvu") == 0) {
            yuvType_ = Yvu420Type_planar;
        }
        else if(strcmp(yuvType, "semiPlanarYvu") == 0) {
            yuvType_ = Yvu420Type_semiPlanar;
        }
        r_yuv_ = NULL;
        frameManager_ = new JsFrameManager();
    }
    ~YuvImageResizer() {
        ttLibC_Yuv420_close(&r_yuv_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 4) {
                puts("コンストラクタの引数は4であるべき");
            }
            else {
                String::Utf8Value yuvType(info[0]->ToString());
                YuvImageResizer *obj = new YuvImageResizer(
                        (const char *)*yuvType,
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value(),
                        info[3]->IsTrue());
                obj->Wrap(info.This());
            }
            info.GetReturnValue().Set(info.This());
        }
        else {
            Local<Value> *argv = new Local<Value>[info.Length()];
            for(int i = 0;i < info.Length();++ i) {
                argv[i] = info[i];
            }
            Local<Function> cons = Nan::New(constructor());
            info.GetReturnValue().Set(Nan::NewInstance(cons, (const int)info.Length(), argv).ToLocalChecked());
            delete[] argv;
        }
    }
    static NAN_METHOD(Resample) {
        if(info.Length() != 2) {
            puts("パラメーターはフレームとcallbackの２つであるべき");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsObject()) {
            puts("1st argはframeオブジェクトでないとだめです。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[1]->IsFunction()) {
            puts("2nd argはfunctionオブジェクトでないとだめです。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        // ここで確認して、すでにresampleする必要がなければ、そのまま応答を返すみたいな動作にしておけばいいと思う。
        YuvImageResizer* resampler = Nan::ObjectWrap::Unwrap<YuvImageResizer>(info.Holder());
        ttLibC_Frame *frame = resampler->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("フレームの復元ができなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != frameType_yuv420) {
            puts("yuv420のみ対応しています。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        ttLibC_Yuv420 *yuv = ttLibC_ImageResizer_resizeYuv420(
                resampler->r_yuv_,
                resampler->yuvType_,
                resampler->width_,
                resampler->height_,
                (ttLibC_Yuv420 *)frame,
                resampler->isQuick_);
        if(yuv == NULL) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        resampler->r_yuv_ = yuv;
        resampler->r_yuv_->inherit_super.inherit_super.id = frame->id;
        auto callback= new Nan ::Callback(info[1].As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)resampler->r_yuv_)) {
            Local<Value> args[] = {
                Nan::New("Jsオブジェクト作成失敗").ToLocalChecked(),
                Nan::Null()};
            callback->Call(2, args);
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        else {
            Local<Value> args[] = {
                Nan::Null(),
                jsFrame};
            callback->Call(2, args);
        }
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(Dump) {
        ttLibC_Allocator_dump();
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_Yuv420_Type yuvType_;
    uint32_t width_;
    uint32_t height_;
    bool isQuick_;
    ttLibC_Yuv420 *r_yuv_;
    JsFrameManager *frameManager_;
#endif
};

NODE_MODULE(yuvImageResizer, YuvImageResizer::Init);