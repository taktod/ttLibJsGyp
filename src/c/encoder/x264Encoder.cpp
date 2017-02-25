// yuvをh264に変換する動作(x264版)
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <x264.h>
#   include <ttLibC/encoder/x264Encoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class X264Encoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("X264Encoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "encode", Encode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("X264Encoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit X264Encoder(
            uint32_t width,
            uint32_t height,
            uint32_t maxQuantizer,
            uint32_t minQuantizer,
            uint32_t bitrate,
            uint32_t bframe,
            const char *preset,
            const char *tune,
            const char *profile) {
        x264_param_t param;
        ttLibC_X264Encoder_getDefaultX264ParamTWithPresetTune(
                &param,
                width,
                height,
                preset,
                tune);
        param.rc.i_qp_max = maxQuantizer;
        param.rc.i_qp_min = minQuantizer;
        param.rc.i_bitrate = bitrate / 1000;
        param.i_bframe = bframe;
        if(x264_param_apply_profile(&param, profile) < 0) {
            puts("x264のprofile設定失敗しました。");
            encoder_ = NULL;
        }
        else {
            encoder_ = ttLibC_X264Encoder_makeWithX264ParamT(&param);
        }
        frameManager_ = new JsFrameManager();
    }
    explicit X264Encoder(
            uint32_t width,
            uint32_t height,
            const char *preset,
            const char *tune,
            const char *profile,
            Local<Object> params) {
        x264_param_t param;
        ttLibC_X264Encoder_getDefaultX264ParamTWithPresetTune(
            &param,
            width,
            height,
            preset,
            tune);
        Local<Array> keys = params->GetOwnPropertyNames();
        for(uint32_t i = 0;i < keys->Length();++ i) {
            Local<Value> key = keys->Get(i);
            Local<Value> value = params->Get(key);
            String::Utf8Value keyName(key->ToString());
            String::Utf8Value valueName(value->ToString());
            if(x264_param_parse(&param, (const char *)*keyName, (const char *)*valueName) != 0) {
                printf("x264_param_parse failed. key:%s value:%s\n", (const char *)*keyName, (const char *)*valueName);
            }
        }
        if(x264_param_apply_profile(&param, profile) < 0) {
            puts("x264のprofile設定失敗しました。");
            encoder_ = NULL;
        }
        else {
            encoder_ = ttLibC_X264Encoder_makeWithX264ParamT(&param);
        }
        frameManager_ = new JsFrameManager();
    }
    ~X264Encoder() {
        ttLibC_X264Encoder_close(&encoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() == 5) {
                // 既存の動作 width height maxQuantizer minQuantizer bitrate
                X264Encoder *encoder = new X264Encoder(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value(),
                        info[3]->Uint32Value(),
                        info[4]->Uint32Value(),
                        0,
                        NULL,
                        NULL,
                        "baseline");
                encoder->Wrap(info.This());
            }
            else if(info.Length() == 6) {
                // width height preset tune profile params
                String::Utf8Value preset(info[2]->ToString());
                String::Utf8Value tune(info[3]->ToString());
                String::Utf8Value profile(info[4]->ToString());
                X264Encoder *encoder = new X264Encoder(
                    info[0]->Uint32Value(),
                    info[1]->Uint32Value(),
                    (const char *)*preset,
                    (const char *)*tune,
                    (const char *)*profile,
                    info[5]->ToObject());
                encoder->Wrap(info.This());
            }
            else if(info.Length() == 9){
                // あたらしいものだったらwidth height maxQuantizer minQuantizer bitrate
                // width height maxQuantizer minQuantizer bitrate
                // bframe preset tune profile
                String::Utf8Value preset(info[6]->ToString());
                String::Utf8Value tune(info[7]->ToString());
                String::Utf8Value profile(info[8]->ToString());
                X264Encoder *encoder = new X264Encoder(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value(),
                        info[3]->Uint32Value(),
                        info[4]->Uint32Value(),
                        info[5]->Uint32Value(),
                        (const char *)*preset,
                        (const char *)*tune,
                        (const char *)*profile);
                encoder->Wrap(info.This());
            }
            else {
                puts("コンストラクタのパラメーター数がおかしいです。");
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
    static bool encodeCallback(void *ptr, ttLibC_H264 *h264) {
        X264Encoder *encoder = (X264Encoder *)ptr;
        auto callback = new Nan::Callback(encoder->callback_.As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)h264)) {
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
        X264Encoder *encoder = Nan::ObjectWrap::Unwrap<X264Encoder>(info.Holder());
        encoder->callback_ = info[1];
        ttLibC_Frame *frame = encoder->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type == frameType_yuv420) {
            if(!ttLibC_X264Encoder_encode(
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
    ttLibC_X264Encoder *encoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(x264Encoder, X264Encoder::Init);