// soundtouchをつかってピッチが速度を変更するリサンプラ
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <ttLibC/resampler/soundtouchResampler.h>
#endif

#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/frame/audio/pcmf32.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class SoundtouchResampler : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("SoundtouchResampler").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "resample",          Resample);
        SetPrototypeMethod(tpl, "setRate",           SetRate);
        SetPrototypeMethod(tpl, "setTempo",          SetTempo);
        SetPrototypeMethod(tpl, "setRateChange",     SetRateChange);
        SetPrototypeMethod(tpl, "setTempoChange",    SetTempoChange);
        SetPrototypeMethod(tpl, "setPitch",          SetPitch);
        SetPrototypeMethod(tpl, "setPitchOctaves",   SetPitchOctaves);
        SetPrototypeMethod(tpl, "setPitchSemiTones", SetPitchSemiTones);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("SoundtouchResampler").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit SoundtouchResampler(
            uint32_t sample_rate,
            uint32_t channel_num) {
        resampler_ = ttLibC_Soundtouch_make(
            sample_rate,
            channel_num);
        frameManager_ = new JsFrameManager();
    }
    ~SoundtouchResampler() {
        ttLibC_Soundtouch_close(&resampler_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 2) {
                puts("コンストラクタの引数は2であるべき");
            }
            else {
                SoundtouchResampler *obj = new SoundtouchResampler(
                    info[0]->Uint32Value(),
                    info[1]->Uint32Value());
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
    static bool resampleCallback(void *ptr, ttLibC_Audio *pcm) {
        SoundtouchResampler *resampler = (SoundtouchResampler *)ptr;
        auto callback = new Nan::Callback(resampler->callback_.As<Function>());
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
        SoundtouchResampler* resampler = Nan::ObjectWrap::Unwrap<SoundtouchResampler>(info.Holder());
        resampler->callback_ = info[1];
        ttLibC_Frame *frame = resampler->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("フレームの復元ができなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != frameType_pcmS16 && frame->type != frameType_pcmF32) {
            puts("pcmのみ対応しています。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!ttLibC_Soundtouch_resample(
                resampler->resampler_,
                (ttLibC_Audio *)frame,
                resampleCallback,
                resampler)) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(SetRate) {
        if(info.Length() != 1) {
            puts("パラメーター設定値１つが必要です。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsNumber()) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        SoundtouchResampler* resampler = Nan::ObjectWrap::Unwrap<SoundtouchResampler>(info.Holder());
        ttLibC_Soundtouch_setRate(resampler->resampler_, info[0]->NumberValue());
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(SetTempo) {
        if(info.Length() != 1) {
            puts("パラメーター設定値１つが必要です。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsNumber()) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        SoundtouchResampler* resampler = Nan::ObjectWrap::Unwrap<SoundtouchResampler>(info.Holder());
        ttLibC_Soundtouch_setTempo(resampler->resampler_, info[0]->NumberValue());
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(SetRateChange) {
        if(info.Length() != 1) {
            puts("パラメーター設定値１つが必要です。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsNumber()) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        SoundtouchResampler* resampler = Nan::ObjectWrap::Unwrap<SoundtouchResampler>(info.Holder());
        ttLibC_Soundtouch_setRateChange(resampler->resampler_, info[0]->NumberValue());
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(SetTempoChange) {
        if(info.Length() != 1) {
            puts("パラメーター設定値１つが必要です。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsNumber()) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        SoundtouchResampler* resampler = Nan::ObjectWrap::Unwrap<SoundtouchResampler>(info.Holder());
        ttLibC_Soundtouch_setTempoChange(resampler->resampler_, info[0]->NumberValue());
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(SetPitch) {
        if(info.Length() != 1) {
            puts("パラメーター設定値１つが必要です。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsNumber()) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        SoundtouchResampler* resampler = Nan::ObjectWrap::Unwrap<SoundtouchResampler>(info.Holder());
        ttLibC_Soundtouch_setPitch(resampler->resampler_, info[0]->NumberValue());
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(SetPitchOctaves) {
        if(info.Length() != 1) {
            puts("パラメーター設定値１つが必要です。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsNumber()) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        SoundtouchResampler* resampler = Nan::ObjectWrap::Unwrap<SoundtouchResampler>(info.Holder());
        ttLibC_Soundtouch_setPitchOctaves(resampler->resampler_, info[0]->NumberValue());
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(SetPitchSemiTones) {
        if(info.Length() != 1) {
            puts("パラメーター設定値１つが必要です。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsNumber()) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        SoundtouchResampler* resampler = Nan::ObjectWrap::Unwrap<SoundtouchResampler>(info.Holder());
        ttLibC_Soundtouch_setPitchSemiTones(resampler->resampler_, info[0]->NumberValue());
        info.GetReturnValue().Set(Nan::New(true));
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_Soundtouch *resampler_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(soundtouchResampler, SoundtouchResampler::Init);