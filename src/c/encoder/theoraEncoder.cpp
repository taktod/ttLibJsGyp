// yuvをtheoraに変換する動作(libtheora利用)
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <ttLibC/encoder/theoraEncoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class TheoraEncoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("TheoraEncoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "encode", Encode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("TheoraEncoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit TheoraEncoder(
            uint32_t width,
            uint32_t height,
            uint32_t quality,
            uint32_t bitrate,
            uint32_t keyFrameInterval) {
        if(quality == 0 && bitrate == 0 && keyFrameInterval == 0) {
            quality = 0;
            bitrate = 320000;
            keyFrameInterval = 6;
        }
        encoder_ = ttLibC_TheoraEncoder_make_ex(
                width,
                height,
                quality,
                bitrate,
                keyFrameInterval);
        frameManager_ = new JsFrameManager();
    }
    ~TheoraEncoder() {
        ttLibC_TheoraEncoder_close(&encoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() == 2) {
                TheoraEncoder *encoder = new TheoraEncoder(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        0,
                        0,
                        0);
                encoder->Wrap(info.This());
            }
            else if(info.Length() == 5) {
                TheoraEncoder *encoder = new TheoraEncoder(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value(),
                        info[3]->Uint32Value(),
                        info[4]->Uint32Value());
                encoder->Wrap(info.This());
            }
            else {
                puts("引数は2か5でお願いします。");
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
    static bool encodeCallback(void *ptr, ttLibC_Theora *theora) {
        TheoraEncoder *encoder = (TheoraEncoder *)ptr;
        auto callback = new Nan::Callback(encoder->callback_.As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)theora)) {
            Local<Value> args[] = {
                Nan::New("Jsオブジェクト作成失敗").ToLocalChecked(),
                Nan::Null()};
            callback->Call(2, args);
        }
        else {
            Local<Value> args[] = {
                Nan::Null(),
                jsFrame};
            callback->Call(2, args);
        }
        return true;
    }
    static NAN_METHOD(Encode) {
        // ここでエンコードを実施する。
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
        TheoraEncoder *encoder = Nan::ObjectWrap::Unwrap<TheoraEncoder>(info.Holder());
        encoder->callback_ = info[1];
        ttLibC_Frame *frame = encoder->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type == frameType_yuv420) {
            if(!ttLibC_TheoraEncoder_encode(
                    encoder->encoder_,
                    (ttLibC_Yuv420 *)frame,
                    encodeCallback,
                    encoder)) {
                info.GetReturnValue().Set(Nan::New(false));
                return;
            }
        }
        else {
            puts("それ以外だ");
        }
        info.GetReturnValue().Set(Nan::New(true));
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_TheoraEncoder *encoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(theoraEncoder, TheoraEncoder::Init);