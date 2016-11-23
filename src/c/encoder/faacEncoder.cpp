// pcmデータをaacにする動作
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>

#ifdef __ENABLE__
#   include <ttLibC/encoder/faacEncoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class FaacEncoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        ttLibC_Allocator_init();
//        FramePassingWorker::Init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("FaacEncoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "encode", Encode);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("FaacEncoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit FaacEncoder(
            ttLibC_FaacEncoder_Type type,
            uint32_t sample_rate,
            uint32_t channel_num,
            uint32_t bitrate) {
        encoder_ = ttLibC_FaacEncoder_make(
                type,
                sample_rate,
                channel_num,
                bitrate);
        frameManager_ = new JsFrameManager();
    }
    ~FaacEncoder() {
        ttLibC_FaacEncoder_close(&encoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 4) {
                puts("コンストラクタの引数は4であるべき");
            }
            else {
                String::Utf8Value type(info[0]->ToString());
                ttLibC_FaacEncoder_Type aacType = FaacEncoderType_Low;
                if(strcmp((const char *)*type, "main") == 0) {
                    aacType = FaacEncoderType_Main;
                }
                else if(strcmp((const char *)*type, "low") == 0) {
                    aacType = FaacEncoderType_Low;
                }
                else if(strcmp((const char *)*type, "ssr") == 0) {
                    aacType = FaacEncoderType_SSR;
                }
                else if(strcmp((const char *)*type, "ltp") == 0) {
                    aacType = FaacEncoderType_LTP;
                }
                FaacEncoder *encoder = new FaacEncoder(
                        aacType,
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value(),
                        info[3]->Uint32Value());
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
    static bool encodeCallback(void *ptr, ttLibC_Aac *aac) {
        FaacEncoder *encoder = (FaacEncoder *)ptr;
        printf("output aac for faac:%lld %d\n", aac->inherit_super.inherit_super.pts, aac->inherit_super.inherit_super.timebase);
        auto callback = new Nan::Callback(encoder->callback_.As<Function>());
//        Nan::AsyncQueueWorker(new FramePassingWorker((ttLibC_Frame *)aac, callback));
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)aac)) {
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
        FaacEncoder *encoder = Nan::ObjectWrap::Unwrap<FaacEncoder>(info.Holder());
        encoder->callback_ = info[1];
        ttLibC_Frame *frame = encoder->frameManager_->getFrame(info[0]->ToObject());
        printf("input pcm for faac:%lld %d\n", frame->pts, frame->timebase);
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type == frameType_pcmS16) {
            if(!ttLibC_FaacEncoder_encode(
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
    static NAN_METHOD(Dump) {
        ttLibC_Allocator_dump();
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_FaacEncoder *encoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(faacEncoder, FaacEncoder::Init);
