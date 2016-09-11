// yuvとbgrの相互変換します。
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>
#include <ttLibC/resampler/imageResampler.h>
#include <ttLibC/frame/video/yuv420.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class ImageResampler : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        ttLibC_Allocator_init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("ImageResampler").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "resample", Resample);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("ImageResampler").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
    explicit ImageResampler(
            const char *type,
            const char *subType) {
        if(strcmp(type, "yuv420") == 0) {
            type_ = frameType_yuv420;
            srcType_ = frameType_bgr;
            puts("yuv420");
            if(strcmp(subType, "planar") == 0) {
                subType_ = Yuv420Type_planar;
            }
            else if(strcmp(subType, "semiPlanar") == 0) {
                subType_ = Yuv420Type_semiPlanar;
            }
            else if(strcmp(subType, "planarYvu") == 0) {
                subType_ = Yvu420Type_planar;
            }
            else if(strcmp(subType, "semiPlanarYvu") == 0) {
                subType_ = Yvu420Type_semiPlanar;
            }
        }
        else if(strcmp(type, "bgr") == 0) {
            puts("bgr");
            type_ = frameType_bgr;
            srcType_ = frameType_yuv420;
            if(strcmp(subType, "bgr") == 0) {
                subType_ = BgrType_bgr;
            }
            else if(strcmp(subType, "bgra") == 0) {
                subType_ = BgrType_bgra;
            }
            else if(strcmp(subType, "abgr") == 0) {
                subType_ = BgrType_abgr;
            }
        }
        r_frame_ = NULL;
        frameManager_ = new JsFrameManager();
    }
    ~ImageResampler() {
        ttLibC_Frame_close(&r_frame_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 2) {
                puts("コンストラクタの引数は2であるべき");
            }
            else {
                String::Utf8Value type(info[0]->ToString());
                String::Utf8Value subType(info[1]->ToString());
                ImageResampler *obj = new ImageResampler(
                        (const char *)*type,
                        (const char *)*subType);
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
        ImageResampler* resampler = Nan::ObjectWrap::Unwrap<ImageResampler>(info.Holder());
        ttLibC_Frame *frame = resampler->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("フレームの復元ができなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != resampler->srcType_) {
            puts("想定入力フレームではないようです。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        switch(frame->type) {
        case frameType_bgr:
            {
                ttLibC_Yuv420 *yuv = ttLibC_ImageResampler_makeYuv420FromBgr(
                    (ttLibC_Yuv420*)resampler->r_frame_,
                    (ttLibC_Yuv420_Type)resampler->subType_,
                    (ttLibC_Bgr *)frame);
                if(yuv == NULL) {
                    puts("yuvデータができませんでした。");
                    info.GetReturnValue().Set(Nan::New(false));
                    return;
                }
                resampler->r_frame_ = (ttLibC_Frame *)yuv;
            }
            break;
        case frameType_yuv420:
            {
                ttLibC_Bgr *bgr = ttLibC_ImageResampler_makeBgrFromYuv420(
                    (ttLibC_Bgr *)resampler->r_frame_,
                    (ttLibC_Bgr_Type)resampler->subType_,
                    (ttLibC_Yuv420 *)frame);
                if(bgr == NULL) {
                    puts("bgrデータができませんでした。");
                    info.GetReturnValue().Set(Nan::New(false));
                    return;
                }
                resampler->r_frame_ = (ttLibC_Frame *)bgr;
            }
            break;
        default:
            puts("想定外のフレームtypeです。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        resampler->r_frame_->id = frame->id;
        auto callback= new Nan ::Callback(info[1].As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)resampler->r_frame_)) {
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
    ttLibC_Frame_Type srcType_; // 変換元フレームタイプ
    ttLibC_Frame_Type type_;    // ターゲットフレームタイプ
    uint32_t          subType_; // ターゲットサブタイプ
    ttLibC_Frame     *r_frame_; // 生成フレーム
    JsFrameManager   *frameManager_;
};

NODE_MODULE(imageResampler, ImageResampler::Init);