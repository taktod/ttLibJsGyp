// openCVを利用して、画像の描画を実施するプログラムをつくっておく。
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>

#ifdef __ENABLE__
#   include <ttLibC/util/opencvUtil.h>
#endif

#include <ttLibC/frame/video/yuv420.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class OpencvWindow : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        ttLibC_Allocator_init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("OpencvWindow").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "show", Show);
        SetPrototypeMethod(tpl, "update", Update);

        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("OpencvWindow").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit OpencvWindow(const char *name) {
        window_ = ttLibC_CvWindow_make(name);
        frameManager_ = new JsFrameManager();
    }
    ~OpencvWindow() {
        ttLibC_CvWindow_close(&window_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            if(info.Length() != 1) {
                puts("コンストラクタの引数は4であるべき");
            }
            else {
                String::Utf8Value name(info[0]->ToString());
                OpencvWindow *obj = new OpencvWindow((const char *)*name);
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
    static NAN_METHOD(Show) {
        if(info.Length() != 1) {
            puts("パラメーターはフレームとcallbackの２つであるべき");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsObject()) {
            puts("1st argはframeオブジェクトでないとだめです。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        // ここで確認して、すでにresampleする必要がなければ、そのまま応答を返すみたいな動作にしておけばいいと思う。
        OpencvWindow* window = Nan::ObjectWrap::Unwrap<OpencvWindow>(info.Holder());
        ttLibC_Frame *frame = window->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("フレームの復元ができなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(frame->type != frameType_bgr) {
            puts("bgrのみ対応しています。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        ttLibC_CvWindow_showBgr(window->window_, (ttLibC_Bgr *)frame);
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(Update) {
        if(info.Length() != 1) {
            puts("パラメーターは1つ");
            info.GetReturnValue().Set(Nan::New(0));
            return;
        }
        if(!info[0]->IsUint32()) {
            puts("入力データは整数でないとだめです。");
            info.GetReturnValue().Set(Nan::New(0));
            return;
        }
        // これいらないのか・・・
//        OpencvWindow* window = Nan::ObjectWrap::Unwrap<OpencvWindow>(info.Holder());
        info.GetReturnValue().Set(Nan::New(ttLibC_CvWindow_waitForKeyInput(info[0]->Uint32Value())));
    }
    static NAN_METHOD(Dump) {
        ttLibC_Allocator_dump();
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_CvWindow *window_;
    JsFrameManager *frameManager_;
#endif
};

NODE_MODULE(opencvWindow, OpencvWindow::Init);