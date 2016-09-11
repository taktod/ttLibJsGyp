// bgrの画像サイズを変更する動作。拡大縮小します。
// なお切り抜きの場合はwidthとheight、dataPosいじれば・・・あ、無理か
// 切り抜きできないね・・・一応dataをいじればできないこともないか・・・
// subarrayでできるのかな。
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>

#ifdef __ENABLE__
#   include <ttLibC/resampler/imageResizer.h>
#endif

#include <ttLibC/frame/video/bgr.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class BgrImageResizer : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        ttLibC_Allocator_init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("BgrImageResizer").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "resample", Resample);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("BgrImageResizer").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit BgrImageResizer(
            const char *bgrType,
            uint32_t    width,
            uint32_t    height) : 
                width_(width), height_(height) {
        if(strcmp(bgrType, "bgr") == 0) {
            bgrType_ = BgrType_bgr;
        }
        else if(strcmp(bgrType, "bgra") == 0) {
            bgrType_ = BgrType_bgra;
        }
        else if(strcmp(bgrType, "abgr") == 0) {
            bgrType_ = BgrType_abgr;
        }
        r_bgr_ = NULL;
        frameManager_ = new JsFrameManager();
    }
    ~BgrImageResizer() {
        ttLibC_Bgr_close(&r_bgr_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 3) {
                puts("コンストラクタの引数は3であるべき");
            }
            else {
                String::Utf8Value bgrType(info[0]->ToString());
                BgrImageResizer *obj = new BgrImageResizer(
                        (const char *)*bgrType,
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value());
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
        BgrImageResizer* resampler = Nan::ObjectWrap::Unwrap<BgrImageResizer>(info.Holder());
        ttLibC_Frame *frame = resampler->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("フレームの復元ができなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != frameType_bgr) {
            puts("bgrのみ対応しています。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        ttLibC_Bgr *bgr = ttLibC_ImageResizer_resizeBgr(
                resampler->r_bgr_,
                resampler->bgrType_,
                resampler->width_,
                resampler->height_,
                (ttLibC_Bgr *)frame);
        if(bgr == NULL) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        resampler->r_bgr_ = bgr;
        resampler->r_bgr_->inherit_super.inherit_super.id = frame->id;
        auto callback= new Nan ::Callback(info[1].As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)resampler->r_bgr_)) {
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
    ttLibC_Bgr_Type bgrType_;
    uint32_t width_;
    uint32_t height_;
    ttLibC_Bgr *r_bgr_;
    JsFrameManager *frameManager_;
#endif
};

NODE_MODULE(bgrImageResizer, BgrImageResizer::Init);