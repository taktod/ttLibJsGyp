// pcmデータをopusにする動作
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <ttLibC/encoder/opusEncoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class OpusEncoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("OpusEncoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "encode", Encode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("OpusEncoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit OpusEncoder(
            uint32_t sample_rate,
            uint32_t channel_num,
            uint32_t unit_sample_num) {
        encoder_ = ttLibC_OpusEncoder_make(sample_rate, channel_num, unit_sample_num);
        frameManager_ = new JsFrameManager();
    }
    ~OpusEncoder() {
        ttLibC_OpusEncoder_close(&encoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 3) {
                puts("コンストラクタの引数は3であるべき");
            }
            else {
                OpusEncoder *encoder = new OpusEncoder(
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
    static bool encodeCallback(void *ptr, ttLibC_Opus *opus) {
        OpusEncoder *encoder = (OpusEncoder *)ptr;
        auto callback = new Nan::Callback(encoder->callback_.As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)opus)) {
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
        OpusEncoder *encoder = Nan::ObjectWrap::Unwrap<OpusEncoder>(info.Holder());
        encoder->callback_ = info[1];
        ttLibC_Frame *frame = encoder->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type == frameType_pcmS16) {
            if(!ttLibC_OpusEncoder_encode(
                    encoder->encoder_,
                    (ttLibC_PcmS16 *)frame,
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
    ttLibC_OpusEncoder *encoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(opusEncoder, OpusEncoder::Init);
