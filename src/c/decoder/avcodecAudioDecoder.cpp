// 音声データを何かしらのpcmにする。libavcodecを使う。
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>

#ifdef __ENABLE__
#	include <ttLibC/decoder/avcodecDecoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class AvcodecAudioDecoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        ttLibC_Allocator_init();
//        FramePassingWorker::Init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("AvcodecAudioDecoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "decode", Decode);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("AvcodecAudioDecoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit AvcodecAudioDecoder(
            uint32_t sample_rate,
            uint32_t channel_num,
            ttLibC_Frame_Type type) {
        decoder_ = ttLibC_AvcodecAudioDecoder_make(type, sample_rate, channel_num);
        frameManager_ = new JsFrameManager();
    }
    ~AvcodecAudioDecoder() {
        ttLibC_AvcodecDecoder_close(&decoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 3) {
                puts("コンストラクタの引数は3であるべき");
            }
            else {
                String::Utf8Value codec(info[2]->ToString());
                ttLibC_Frame_Type type = frameType_unknown;
                if(strcmp((const char *)*codec, "aac") == 0) {
                    type = frameType_aac;
                }
                else if(strcmp((const char *)*codec, "mp3") == 0) {
                    type = frameType_mp3;
                }
                AvcodecAudioDecoder *decoder = new AvcodecAudioDecoder(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        type);
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
    static bool decodeCallback(void *ptr, ttLibC_Frame *pcm) {
        AvcodecAudioDecoder *decoder = (AvcodecAudioDecoder *)ptr;
        auto callback = new Nan::Callback(decoder->callback_.As<Function>());
//        Nan::AsyncQueueWorker(new FramePassingWorker((ttLibC_Frame *)pcm, callback));
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                pcm)) {
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
        AvcodecAudioDecoder *decoder = Nan::ObjectWrap::Unwrap<AvcodecAudioDecoder>(info.Holder());
        decoder->callback_ = info[1];
        // フレーム取得
        ttLibC_Frame *frame = decoder->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != decoder->decoder_->frame_type) {
            info.GetReturnValue().Set(Nan::New(true));
            return;
        }
        if(!ttLibC_AvcodecDecoder_decode(
                decoder->decoder_,
                frame,
                decodeCallback,
                decoder)) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
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
    ttLibC_AvcodecDecoder *decoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(avcodecAudioDecoder, AvcodecAudioDecoder::Init);