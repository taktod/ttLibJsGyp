// yuvをx265に変換する動作(x265版)
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <x265.h>
#   include <ttLibC/encoder/x265Encoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class X265Encoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("X265Encoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "encode", Encode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("X265Encoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit X265Encoder(
            uint32_t width,
            uint32_t height,
            uint32_t bitrate) {
        encoder_ = ttLibC_X265Encoder_make_ex(
                width,
                height,
                bitrate);
        frameManager_ = new JsFrameManager();
    }
    explicit X265Encoder(
            uint32_t width,
            uint32_t height,
            const char *preset,
            const char *tune,
            const char *profile,
            Local<Object> params) {
        x265_api *api;
        x265_param *param;
        encoder_ = NULL;
        if(!ttLibC_X265Encoder_getDefaultX265ApiAndParam(
                (void **)&api, (void **)&param,
                preset, tune,
                width, height)) {
            puts("preset tuneの設定失敗");
        }
        else {
            Local<Array> keys = params->GetOwnPropertyNames();
            for(uint32_t i = 0;i < keys->Length();++ i) {
                Local<Value> key = keys->Get(i);
                Local<Value> value = params->Get(key);
                String::Utf8Value keyName(key->ToString());
                String::Utf8Value valueName(value->ToString());
                if(x265_param_parse(param, (const char *)*keyName, (const char *)*valueName) != 0) {
                    printf("x265_param_parse failed. key:%s value:%s\n", (const char *)*keyName, (const char *)*valueName);
                }
            }
            if(x265_param_apply_profile(param, profile) < 0) {
                puts("profile設定失敗");
            }
            else {
                encoder_ = ttLibC_X265Encoder_makeWithX265ApiAndParam(api, param);
            }
        }
        frameManager_ = new JsFrameManager();
    }
    ~X265Encoder() {
        ttLibC_X265Encoder_close(&encoder_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() == 3) {
                X265Encoder *encoder = new X265Encoder(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value());
                encoder->Wrap(info.This());
            }
            else if(info.Length() == 6) {
                // width height preset tune profile params
                String::Utf8Value preset(info[2]->ToString());
                String::Utf8Value tune(info[3]->ToString());
                String::Utf8Value profile(info[4]->ToString());
                X265Encoder *encoder = new X265Encoder(
                    info[0]->Uint32Value(),
                    info[1]->Uint32Value(),
                    (const char *)*preset,
                    (const char *)*tune,
                    (const char *)*profile,
                    info[5]->ToObject());
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
    static bool encodeCallback(void *ptr, ttLibC_H265 *h265) {
        X265Encoder *encoder = (X265Encoder *)ptr;
        auto callback = new Nan::Callback(encoder->callback_.As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)h265)) {
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
        X265Encoder *encoder = Nan::ObjectWrap::Unwrap<X265Encoder>(info.Holder());
        encoder->callback_ = info[1];
        ttLibC_Frame *frame = encoder->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type == frameType_yuv420) {
            if(!ttLibC_X265Encoder_encode(
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
    ttLibC_X265Encoder *encoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(x265Encoder, X265Encoder::Init);