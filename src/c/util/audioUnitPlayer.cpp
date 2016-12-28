// 音声の再生からつくるか・・・
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <ttLibC/util/audioUnitUtil.h>
#endif

#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/frame/frame.h>
#include <pthread.h>
#include <stdlib.h>
//#include <unistd.h>

using namespace v8;

class AudioUnitPlayer: public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("AudioUnitPlayer").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "queue", Queue);
        SetPrototypeMethod(tpl, "getPts", GetPts);
        SetPrototypeMethod(tpl, "getTimebase", GetTimebase);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("AudioUnitPlayer").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit AudioUnitPlayer(
            uint32_t sampleRate,
            uint32_t channelNum) { // デバイス選択はなくてもいいかな・・・そこまでなプログラム組むつもりはない。
        auPlayer_ = ttLibC_AuPlayer_make(sampleRate, channelNum, AuPlayerType_DefaultOutput);
        frameManager_ = new JsFrameManager();
    }
    ~AudioUnitPlayer() {
        ttLibC_AuPlayer_close(&auPlayer_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 2) {
                puts("コンストラクタの引数は2であるべき");
            }
            else {
                AudioUnitPlayer *obj = new AudioUnitPlayer(
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
    // このqueueの動作はblockしないことにしておこう。
    // いや、やっぱりブロックしちゃおう。
    // 応答かえしちゃおう。
    static NAN_METHOD(Queue) {
        if(info.Length() != 1) {
            puts("パラメーターはフレームであるべき");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsObject()) {
            puts("1st argはframeオブジェクトでないとだめです。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        AudioUnitPlayer *player = Nan::ObjectWrap::Unwrap<AudioUnitPlayer>(info.Holder());
        ttLibC_Frame *frame = player->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("フレームの復元ができなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != frameType_pcmS16) {
            puts("pcmS16のみ対応しています。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        // falseだったらあとでもう一度入力しなければいけないことになるかも。
        // ここはlockではなくて、return応答にすればいいかも。
        info.GetReturnValue().Set(Nan::New(ttLibC_AuPlayer_queue(player->auPlayer_, (ttLibC_PcmS16 *)frame)));
    }
    static NAN_METHOD(GetPts) {
        // ptsの進行具合を参照する。
        AudioUnitPlayer *player = Nan::ObjectWrap::Unwrap<AudioUnitPlayer>(info.Holder());
        info.GetReturnValue().Set(Nan::New((float)ttLibC_AuPlayer_getPts(player->auPlayer_)));
    }
    static NAN_METHOD(GetTimebase) {
        // timebaseを参照する。
        AudioUnitPlayer *player = Nan::ObjectWrap::Unwrap<AudioUnitPlayer>(info.Holder());
        info.GetReturnValue().Set(Nan::New(ttLibC_AuPlayer_getTimebase(player->auPlayer_)));
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    JsFrameManager *frameManager_;
    ttLibC_AuPlayer *auPlayer_;
#endif
};

NODE_MODULE(audioUnitPlayer, AudioUnitPlayer::Init);