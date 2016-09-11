// openCVを利用して、画像の描画を実施するプログラムをつくっておく。
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>

#ifdef __ENABLE__
#   include <ttLibC/util/opencvUtil.h>
#endif

#include <ttLibC/frame/video/bgr.h>
#include <ttLibC/frame/frame.h>
#include <pthread.h>
#include <stdlib.h>

using namespace v8;

#ifdef __ENABLE__

class AsyncShowWorker : public Nan::AsyncWorker {
public:
    AsyncShowWorker(
        ttLibC_CvWindow *window,
        ttLibC_Bgr *frame,
        pthread_mutex_t *mutex)
             : Nan::AsyncWorker(NULL), window_(window), mutex_(mutex) {
        bgr_ = ttLibC_Bgr_clone(NULL, frame);
    }
    void Execute() {
        puts("execute");
        // ここで実行すればよい。
        // ここにmutexをいれれておく。
        // 表示実行するけど、競合したらいやなので、mutexで管理だけしとくか・・・
        int r = pthread_mutex_lock(mutex_);
        if(r == 0) {
            printf("window:%d bgr:%d\n", window_, bgr_);
            puts("実行します。");
            ttLibC_CvWindow_showBgr(window_, bgr_);
            // 表示の更新をこっちで実施するとだめみたい。
            // mainThreadでする必要がある的な感じだ。
//            ttLibC_CvWindow_waitForKeyInput(1); // とりあえず待ってみよう。
            puts("処理おわり。");
            r = pthread_mutex_unlock(mutex_);
            if(r != 0) {
                puts("failed to unlock.");
                return;
            }
        }
        else {
            puts("failed to lock.");
        }
        // 処理おわったら、bgrを解放しておく。
        ttLibC_Bgr_close(&bgr_);
        puts("全部おわり");
    }
    void HandleOKCallback() {
        puts("done");
        // なんか応答すべきことあるか？
        // 特にない。
    }
private:
    // ここで必要になる、mutex
    pthread_mutex_t *mutex_;
    ttLibC_CvWindow *window_;
    ttLibC_Bgr *bgr_;
};

#endif

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
        pthread_mutex_init(&mutex_, NULL);
        window_ = ttLibC_CvWindow_make(name);
        frameManager_ = new JsFrameManager();
    }
    ~OpencvWindow() {
        ttLibC_CvWindow_close(&window_);
        delete frameManager_;
        pthread_mutex_destroy(&mutex_);
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
        Nan::AsyncQueueWorker(new AsyncShowWorker(window->window_, (ttLibC_Bgr *)frame, &window->mutex_));
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
        OpencvWindow* window = Nan::ObjectWrap::Unwrap<OpencvWindow>(info.Holder());
        int res = 0;
        int r = pthread_mutex_lock(&window->mutex_);
        if(r == 0) {
            puts("keyInputWaitします。(表示の更新)");
            res = ttLibC_CvWindow_waitForKeyInput(info[0]->Uint32Value()); // とりあえず待ってみよう。
            r = pthread_mutex_unlock(&window->mutex_);
            if(r != 0) {
                puts("failed to unlock.");
                return;
            }
        }
        else {
            puts("failed to lock.");
        }
        info.GetReturnValue().Set(Nan::New(res));
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
    pthread_mutex_t mutex_; // ここでmutexをつくっておけばいいか・・・
#endif
};

NODE_MODULE(opencvWindow, OpencvWindow::Init);