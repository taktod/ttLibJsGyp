// pcmのtypeを切り替えるresampler
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/resampler/audioResampler.h>
#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/frame/audio/pcmf32.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class AudioResampler : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("AudioResampler").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "resample", Resample);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("AudioResampler").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
    /**
     * コンストラクタ変換後のフォーマットの指定をする。
     * @param type pcmS16もしくはpcmF32
     * @param pcmType pcmS16の場合は littleEndian bigEndian littleEndian_planar bigEndian_planar(bigEndianはまだ用途がないので未実装)
     *                pcmF32の場合は interleave planarのいずれか
     */
    explicit AudioResampler(
            const char *type,
            const char *pcmType) {
        if(strcmp(type, "pcmS16") == 0) {
            type_ = frameType_pcmS16;
            if(strcmp(pcmType, "littleEndian") == 0) {
                pcmType_ = PcmS16Type_littleEndian;
            }
            else if(strcmp(pcmType, "bigEndian") == 0) {
                pcmType_ = PcmS16Type_bigEndian;
            }
            else if(strcmp(pcmType, "littleEndian_planar") == 0) {
                pcmType_ = PcmS16Type_littleEndian_planar;
            }
            else if(strcmp(pcmType, "bigEndian_planar") == 0) {
                pcmType_ = PcmS16Type_bigEndian_planar;
            }
        }
        else if(strcmp(type, "pcmF32") == 0){
            puts("pcmF32");
            type_ = frameType_pcmF32;
            if(strcmp(pcmType, "interleave") == 0) {
                pcmType_ = PcmF32Type_interleave;
            }
            else if(strcmp(pcmType, "planar") == 0) {
                pcmType_ = PcmF32Type_planar;
            }
        }
        else {
            type_ = frameType_unknown;
        }
        r_pcm_ = NULL;
        frameManager_ = new JsFrameManager();
    }
    ~AudioResampler() {
        ttLibC_Frame_close(&r_pcm_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            // コンストラクタの状態で出力をどうするか決定しておいて
            // resampleのときには、入力pcmとcallbackだけで済むようにしておく。
            // typeとpcmTypeの２つでいいかな。
            String::Utf8Value type(info[0]->ToString());
            String::Utf8Value pcmType(info[1]->ToString());
            AudioResampler *obj = new AudioResampler((const char *)*type, (const char *)*pcmType);
            obj->Wrap(info.This());
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
        AudioResampler* resampler = Nan::ObjectWrap::Unwrap<AudioResampler>(info.Holder());
        ttLibC_Frame *frame = resampler->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("フレームの復元ができなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        ttLibC_Audio *audio = (ttLibC_Audio *)frame;
        // resample実施しなければ・・・
        ttLibC_Audio *resampled_audio = ttLibC_AudioResampler_convertFormat(
                (ttLibC_Audio *)resampler->r_pcm_,
                resampler->type_,
                resampler->pcmType_,
                audio->channel_num,
                audio);
        if(resampled_audio == NULL) {
            puts("resampleできてない。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        resampler->r_pcm_ = (ttLibC_Frame *)resampled_audio;
        resampler->r_pcm_->id = frame->id;
        auto callback= new Nan ::Callback(info[1].As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                resampler->r_pcm_)) {
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
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_Frame_Type type_;
    uint32_t pcmType_;
    ttLibC_Frame *r_pcm_;
    JsFrameManager *frameManager_;
};

NODE_MODULE(audioResampler, AudioResampler::Init);
