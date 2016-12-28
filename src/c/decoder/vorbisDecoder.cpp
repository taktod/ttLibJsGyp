// vorbisデータをpcmf32にする。
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <ttLibC/decoder/vorbisDecoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class VorbisDecoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("VorbisDecoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "decode", Decode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("VorbisDecoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit VorbisDecoder() {
        decoder_ = ttLibC_VorbisDecoder_make();
        frameManager_ = new JsFrameManager();
    }
    ~VorbisDecoder() {
        ttLibC_VorbisDecoder_close(&decoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 0) {
                puts("コンストラクタの引数は0であるべき");
            }
            else {
                VorbisDecoder *decoder = new VorbisDecoder();
                decoder->Wrap(info.This());
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
    static bool decodeCallback(void *ptr, ttLibC_PcmF32 *pcm) {
        VorbisDecoder *decoder = (VorbisDecoder *)ptr;
        auto callback = new Nan::Callback(decoder->callback_.As<Function>());
//        Nan::AsyncQueueWorker(new FramePassingWorker((ttLibC_Frame *)pcm, callback));
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)pcm)) {
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
        VorbisDecoder *decoder = Nan::ObjectWrap::Unwrap<VorbisDecoder>(info.Holder());
        decoder->callback_ = info[1];
        // フレーム取得
        ttLibC_Frame *frame = decoder->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != frameType_vorbis) {
            info.GetReturnValue().Set(Nan::New(true));
            return;
        }
        if(!ttLibC_VorbisDecoder_decode(
                decoder->decoder_,
                (ttLibC_Vorbis *)frame,
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
    ttLibC_VorbisDecoder *decoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(vorbisDecoder, VorbisDecoder::Init);