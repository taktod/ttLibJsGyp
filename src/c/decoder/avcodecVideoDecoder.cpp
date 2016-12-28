// 映像データをyuvかbgrに変換する。libavcodecを使う。
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#	include <ttLibC/decoder/avcodecDecoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class AvcodecVideoDecoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("AvcodecVideoDecoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "decode", Decode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("AvcodecVideoDecoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit AvcodecVideoDecoder(
            uint32_t width,
            uint32_t height,
            ttLibC_Frame_Type type) {
        decoder_ = ttLibC_AvcodecVideoDecoder_make(type, width, height);
        frameManager_ = new JsFrameManager();
    }
    ~AvcodecVideoDecoder() {
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
                if(strcmp((const char *)*codec, "h264") == 0) {
                    type = frameType_h264;
                }
                else if(strcmp((const char *)*codec, "h265") == 0) {
                    type = frameType_h265;
                }
                else if(strcmp((const char *)*codec, "flv1") == 0) {
                    type = frameType_flv1;
                }
                else if(strcmp((const char *)*codec, "jpeg") == 0) {
                    type = frameType_jpeg;
                }
                else if(strcmp((const char *)*codec, "theora") == 0) {
                    type = frameType_theora;
                }
                else if(strcmp((const char *)*codec, "vp6") == 0) {
                    type = frameType_vp6;
                }
                else if(strcmp((const char *)*codec, "vp8") == 0) {
                    type = frameType_vp8;
                }
                else if(strcmp((const char *)*codec, "vp9") == 0) {
                    type = frameType_vp9;
                }
                AvcodecVideoDecoder *decoder = new AvcodecVideoDecoder(
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
    static bool decodeCallback(void *ptr, ttLibC_Frame *video) {
        AvcodecVideoDecoder *decoder = (AvcodecVideoDecoder *)ptr;
        auto callback = new Nan::Callback(decoder->callback_.As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                video)) {
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
        AvcodecVideoDecoder *decoder = Nan::ObjectWrap::Unwrap<AvcodecVideoDecoder>(info.Holder());
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
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_AvcodecDecoder *decoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(avcodecVideoDecoder, AvcodecVideoDecoder::Init);