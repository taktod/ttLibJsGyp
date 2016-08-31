// sampleRateを切り替えるresampler speexdspを使います。
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>

#ifdef __ENABLE__
#   include <ttLibC/resampler/speexdspResampler.h>
#endif

#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class SpeexdspResampler : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        ttLibC_Allocator_init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("SpeexdspResampler").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "resample", Resample);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("SpeexdspResampler").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit SpeexdspResampler(
            uint32_t channel_num,
            uint32_t input_sample_rate,
            uint32_t output_sample_rate,
            uint32_t quality) {
        resampler_ = ttLibC_SpeexdspResampler_make(
                channel_num,
                input_sample_rate,
                output_sample_rate,
                quality);
        r_pcm_ = NULL;
        frameManager_ = new JsFrameManager();
    }
    ~SpeexdspResampler() {
        ttLibC_SpeexdspResampler_close(&resampler_);
        ttLibC_PcmS16_close(&r_pcm_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 4) {
                puts("コンストラクタの引数は4であるべき");
            }
            else {
                // コンストラクタの状態で出力をどうするか決定しておいて
                // resampleのときには、入力pcmとcallbackだけで済むようにしておく。
                // typeとpcmTypeの２つでいいかな。
                SpeexdspResampler *obj = new SpeexdspResampler(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value(),
                        info[3]->Uint32Value());
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
        SpeexdspResampler* resampler = Nan::ObjectWrap::Unwrap<SpeexdspResampler>(info.Holder());
        ttLibC_Frame *frame = resampler->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("フレームの復元ができなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != frameType_pcmS16) {
            puts("pcmS16 litte endian interleaveのみ対応しています。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        ttLibC_PcmS16 *resampled = ttLibC_SpeexdspResampler_resample(
                resampler->resampler_,
                resampler->r_pcm_,
                (ttLibC_PcmS16 *)frame);
        if(resampled == NULL) {
            info.GetReturnValue().Set(Nan::New(true));
            return;
        }
        resampler->r_pcm_ = resampled;
        resampler->r_pcm_->inherit_super.inherit_super.id = frame->id;
        auto callback= new Nan ::Callback(info[1].As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)resampler->r_pcm_)) {
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
    ttLibC_PcmS16 *r_pcm_;
    ttLibC_SpeexdspResampler *resampler_;
    JsFrameManager *frameManager_;
#endif
};

NODE_MODULE(speexdspResampler, SpeexdspResampler::Init);