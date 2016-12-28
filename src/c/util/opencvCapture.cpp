// opencvをつかってcapture画像を追加
#include <nan.h>
#include "../frame/frame.hpp"

#ifdef __ENABLE__
#   include <ttLibC/util/opencvUtil.h>
#endif

#include <ttLibC/frame/video/bgr.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class OpencvCapture : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("OpencvCapture").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "query", Query);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("OpencvCapture").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit OpencvCapture(
            uint32_t cameraNum,
            uint32_t width,
            uint32_t height) {
        capture_ = ttLibC_CvCapture_make(
            cameraNum,
            width,
            height);
        bgr_ = NULL;
    }
    ~OpencvCapture() {
        ttLibC_Bgr_close(&bgr_);
        ttLibC_CvCapture_close(&capture_);
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 3) {
                puts("コンストラクタの引数は3であるべき");
            }
            else {
                OpencvCapture *obj = new OpencvCapture(
                        info[0]->Uint32Value(),
                        info[1]->Uint32Value(),
                        info[2]->Uint32Value());
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
    static NAN_METHOD(Query) {
        if(info.Length() != 1) {
            puts("パラメーターはフレームとcallbackの1つであるべき");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsFunction()) {
            puts("1st argはfunctionオブジェクトでないとだめです。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        // ここで確認して、すでにresampleする必要がなければ、そのまま応答を返すみたいな動作にしておけばいいと思う。
        OpencvCapture* capture = Nan::ObjectWrap::Unwrap<OpencvCapture>(info.Holder());
        ttLibC_Bgr *bgr = ttLibC_CvCapture_queryFrame(capture->capture_, capture->bgr_);
        if(bgr == NULL) {
            puts("captureから取得できませんでした。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        auto callback = new Nan::Callback(info[0].As<Function>());
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                (ttLibC_Frame *)bgr)) {
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
        info.GetReturnValue().Set(Nan::New(true));
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_CvCapture *capture_;
    ttLibC_Bgr *bgr_;
#endif
};

NODE_MODULE(opencvCapture, OpencvCapture::Init);
