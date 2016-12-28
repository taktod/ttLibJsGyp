// theoraをyuvにする動作
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <ttLibC/decoder/theoraDecoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class TheoraDecoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("TheoraDecoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "decode", Decode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("TheoraDecoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit TheoraDecoder() {
        decoder_ = ttLibC_TheoraDecoder_make();
        frameManager_ = new JsFrameManager();
    }
    ~TheoraDecoder() {
        ttLibC_TheoraDecoder_close(&decoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            TheoraDecoder *decoder = new TheoraDecoder();
            decoder->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        }
        else {
            const int argc = 0;
            Local<Value> argv[argc] = {};
            Local<Function> cons = Nan::New(constructor());
            info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
        }
    }
    static bool decodeCallback(void *ptr, ttLibC_Yuv420 *yuv) {
        TheoraDecoder *decoder = (TheoraDecoder *)ptr;
        auto callback = new Nan::Callback(decoder->callback_.As<Function>());
//        Nan::AsyncQueueWorker(new FramePassingWorker((ttLibC_Frame *)yuv, callback));
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)yuv)) {
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
    static NAN_METHOD(Decode) {
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
        TheoraDecoder* decoder = Nan::ObjectWrap::Unwrap<TheoraDecoder>(info.Holder());
        decoder->callback_ = info[1];
        // フレーム取得
        ttLibC_Frame *frame = decoder->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != frameType_theora) {
            info.GetReturnValue().Set(Nan::New(true));
            return;
        }
        if(!ttLibC_TheoraDecoder_decode(
                decoder->decoder_,
                (ttLibC_Theora *)frame,
                decodeCallback,
                decoder)) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        info.GetReturnValue().Set(Nan::New(true));
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_TheoraDecoder *decoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(theoraDecoder, TheoraDecoder::Init);