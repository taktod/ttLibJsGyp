// yuvをjpegデータに変換する動作
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <ttLibC/encoder/jpegEncoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class JpegEncoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("JpegEncoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "encode", Encode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("JpegEncoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit JpegEncoder(
            uint32_t width,
            uint32_t height,
            uint32_t quality) {
        encoder_ = ttLibC_JpegEncoder_make(width, height, quality);
        frameManager_ = new JsFrameManager();
    }
    ~JpegEncoder() {
        ttLibC_JpegEncoder_close(&encoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 3) {
                puts("コンストラクタの引数は3であるべき");
            }
            else {
                JpegEncoder *encoder = new JpegEncoder(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value());
                encoder->Wrap(info.This());
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
    static bool encodeCallback(void *ptr, ttLibC_Jpeg *jpeg) {
        JpegEncoder *encoder = (JpegEncoder *)ptr;
        auto callback = new Nan::Callback(encoder->callback_.As<Function>());
//        Nan::AsyncQueueWorker(new FramePassingWorker((ttLibC_Frame *)jpeg, callback));
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)jpeg)) {
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
        JpegEncoder *encoder = Nan::ObjectWrap::Unwrap<JpegEncoder>(info.Holder());
        encoder->callback_ = info[1];
        ttLibC_Frame *frame = encoder->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type == frameType_yuv420) {
            if(!ttLibC_JpegEncoder_encode(
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
    ttLibC_JpegEncoder *encoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(jpegEncoder, JpegEncoder::Init);