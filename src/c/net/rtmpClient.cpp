// とりあえず作ってみたけど、ひどいプログラムになってきたので、なんとかしようと思う。
/*
 * ひどい理由
 * RtmpClientというクラスがベースになっているのに、このクラス・・・netStreamとnetConnectionを両方背負ってる。そして片方がもう一方を継承しているわけでもない。
 * NetConnection側の動作がマルチスレッドでデータの受け取りを実施しているので、callbackがあったときにそっちの動作をとめて、javascriptの世界のthreadから
 * callしなければいけないのに、NetStreamだけがcallされるcallbackが存在する。
 * このせいでエレガントではない。
 *
 * 結論：
 * クラスはやっぱりわけた方がベター
 * NetStream NetConnectionそれぞれのクラスをつくる。
 * イベントのポーリングを実施するためのクラスをまた別につくっておく。
 * 片方が依存するなど言語道断
 * イベントリストな解放はつくらないことにしておく。(現状つくれるイメージがわかない)
 * としておくべき。
 */

#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>
#ifdef __ENABLE__
#   include <ttLibC/net/client/rtmp.h>
#endif
#include <ttLibC/frame/frame.h>
#include <ttLibC/util/stlListUtil.h>
#include <ttLibC/util/amfUtil.h>
#include <stdlib.h>
#include <string.h>

using namespace v8;

#ifdef __ENABLE__

typedef struct rtmpEventList_t rtmpEventList_t;
static Local<Value> makeJsObject_from_ttLibCAmf0(ttLibC_Amf0Object *src_obj);

class RtmpClientBase : public Nan::ObjectWrap {
public:
    virtual void doUpdate() {
        // データのアップデートを実施する。
    }
    virtual void doCallback(rtmpEventList_t *event) {
        // callbackを発動させる。
    }
    bool isCalled_;
};

// イベントリスナを作る上で必要になるもの。
struct rtmpEventList_t{
    RtmpClientBase *baseObject; // NetConnectionやNetStream等イベントのベースになる、クラスインスタンス保持
    char name[256];
    long callbackId; // callbackのハッシュIDできなものをもたせておけばあとでremoveも可能になるわけだが・・・
    Nan::Callback *callback;
};

/**
 * eventを管理するクラス
 * このクラスはNetConnectionとNetStreamでstaticとして保持させようかとおもったけど、
 * 複数のrtmpを同時に扱うプログラムになった場合にまずくなるか・・・
 * というわけで、connectionをつうったときに、EventManagerをつくっておく。
 * streamをつくったときには、connectionからEventManagerをうけとっておく。
 * ということにしておこう。
 * callbackもこいつに請け負わせておいた方が楽になるかな。
 * // これ・・・別に普通にNetConnectionに負わせておいてもいいような気がしてきた・・・
 */
class EventManager {
public:
    explicit EventManager(RtmpClientBase *base) : base_(base) {
        eventList_ = ttLibC_StlList_make();
        isCalled_ = false;
        hasError_ = false;
    }
    ~EventManager() {
    }
    // update処理を実施する。
    void doUpdate() {
        base_->doUpdate();
    }
    // callbackの実行を実施する。
    void doCallback() {
        // forEachでrtmpEventList_tの内容をチェックしていって、RtmpClientBaseに対してdoCallbackを発動させなければならない。
        ttLibC_StlList_forEach(eventList_, doCallbackCallback, this);
        isCalled_ = false; // callの処理がおわったので、フラグを折っておく
    }
    // イベントリスナーをシステムに追加する。
    void addEventListener(RtmpClientBase *baseObject, Nan::NAN_METHOD_ARGS_TYPE info, const char *name) {
        if(name == NULL) {
            if(info.Length() < 2) {
                // パラメーターは２つ必要
                info.GetReturnValue().Set(false);
                puts("パラメーター数が小さい");
                return;
            }
            if(!info[0]->IsString()) {
                puts("1つ目のパラメーターは文字列");
                info.GetReturnValue().Set(false);
                return;
            }
            if(!info[1]->IsFunction()) {
                puts("2つ目のパラメーターは関数"); 
                info.GetReturnValue().Set(false);
                return;
            }
        }
        else {
            if(info.Length() < 1) {
                // パラメーターは1つ必要
                info.GetReturnValue().Set(false);
                puts("パラメーター数が小さい");
                return;
            }
            if(!info[0]->IsFunction()) {
                puts("1つ目のパラメーターは関数"); 
                info.GetReturnValue().Set(false);
                return;
            }
        }
        rtmpEventList_t *event = (rtmpEventList_t *)ttLibC_malloc(sizeof(rtmpEventList_t));
        // イベント名
        if(name == NULL) {
            v8::String::Utf8Value str(info[0]->ToString());
            strncpy(event->name, (const char *)(*str), strlen((const char *)(*str)));
//            sprintf(event->name, (const char *)(*str));
            auto callback = new Nan::Callback(info[1].As<Function>());
            event->callback = callback;
        }
        else {
            strncpy(event->name, name, strlen(name));
//            sprintf(event->name, name);
            auto callback = new Nan::Callback(info[0].As<Function>());
            event->callback = callback;
        }
        // 呼び出しcallback
        event->baseObject = baseObject;
        ttLibC_StlList_addLast(eventList_, event);
    }
    bool isCalled_;
    bool hasError_;
private:
    static bool doCallbackCallback(void *ptr, void *item) {
        rtmpEventList_t *eventList = (rtmpEventList_t *)item;
        eventList->baseObject->doCallback(eventList);
        return true;
    }
    static bool clearAllEventCallback(void *ptr, void *item) {
        if(item != NULL) {
            rtmpEventList_t *eventList = (rtmpEventList_t *)item;
            if(eventList->callback) {
                delete(eventList->callback);
            }
            ttLibC_free(item);
        }
        return true;
    }
    ttLibC_StlList *eventList_;
    RtmpClientBase *base_;
};

class AsyncEventWorker : public Nan::AsyncWorker {
public:
    AsyncEventWorker(EventManager *manager) : 
                Nan::AsyncWorker(NULL), manager_(manager) {
    }
    void Execute() {
        manager_->doUpdate();
    }
    void HandleOKCallback() {
        // 自身をもう一度callするようにする必要があるか
        manager_->doCallback(); // callbackを発動させる。
        if(!manager_->hasError_) {
            Nan::AsyncQueueWorker(new AsyncEventWorker(manager_));
        }
    }
private:
    EventManager *manager_;
};

static bool clearAllAmf0Callback(void *ptr, void *item) {
    ttLibC_Amf0Object *amf0_obj = (ttLibC_Amf0Object *)item;
    ttLibC_Amf0_close(&amf0_obj);
    return true;
}
// 以下作り直していく。
class NetConnection : public RtmpClientBase {
public:
    static void moduleInit(Local<Object> target) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("NetConnection").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "connect", Connect);
        SetPrototypeMethod(tpl, "addEventListener", AddEventListener);
        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

        Nan::Set(
            target,
            Nan::New("NetConnection").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
    void doUpdate() {
        // update処理を実施する。
//        while(true) {
            if(!ttLibC_RtmpConnection_update(conn_, 10000)) {
                // 処理でエラーが発生したら抜ける。
                eventManager_->hasError_ = true;
//                break;
                return;
            }
            if(eventManager_->isCalled_) {
                // managerがcallされた場合 処理を抜ける。
                // mutexいれるべきか？ほんのちょっとだけ心配。
                return;
            }
//        }
//        eventManager_->hasError_ = true;
    }
    void doCallback(rtmpEventList_t *event) {
        // ここで発動させるわけだが・・・
        if(!isCalled_) {
            // calledのフラグが立っていない場合は、今回呼ばれるべき処理ではなかったということ
            return;
        }
        // 呼び出すわけだが・・・
        Local<Object> jsObject = Nan::New<Object>();
        Nan::Set(jsObject, Nan::New("info").ToLocalChecked(), makeJsObject_from_ttLibCAmf0(callObject_));
        Local<Value> args[] = {
            jsObject
        };
        event->callback->Call(1, args);
        isCalled_ = false;
    }
    EventManager *eventManager_;
    ttLibC_RtmpConnection *conn_; // rtmpのコネクション
private:
    explicit NetConnection() {
        conn_ = ttLibC_RtmpConnection_make();
        // callbackを登録
        ttLibC_RtmpConnection_addEventListener(conn_, _onStatusEventCallback, this);
        isCalled_ = false;
        callObject_ = NULL;
        eventManager_ = new EventManager(this);
    }
    ~NetConnection() {
        ttLibC_RtmpConnection_close(&conn_);
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            NetConnection *conn = new NetConnection();
            conn->Wrap(info.This());
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
    static bool _onStatusEventCallback(
            void *ptr,
            ttLibC_Amf0Object *obj) {
        // このcallbackが連続でくることがあるのか、その場合は、２度実行しなければならなくなるが・・・
        NetConnection *conn = (NetConnection *)ptr;
        // callback呼ぶべき
        conn->isCalled_ = true;
        conn->eventManager_->isCalled_ = true;
        // 前の渡していたamf0データがあれば解放
        ttLibC_Amf0_close(&conn->callObject_);
        // 渡すamf0データを記録
        conn->callObject_ = ttLibC_Amf0_clone(obj);
        return true;
    }
    /**
     * 接続する
     * @param address string
     */
    static NAN_METHOD(Connect) {
        // アドレスをパラメーターとして保持
        if(info.Length() != 1) {
            puts("パラメーターは1つ(文字列で)");
            info.GetReturnValue().Set(false);
            return;
        }
        if(!info[0]->IsString()) {
            puts("１つ目のパラメーターは文字列でお願いします。");
            info.GetReturnValue().Set(false);
            return;
        }
        NetConnection* conn = Nan::ObjectWrap::Unwrap<NetConnection>(info.Holder());
        // 接続実施
        v8::String::Utf8Value str(info[0]->ToString());
        ttLibC_RtmpConnection_connect(conn->conn_, (const char *)(*str));
        // Workerをつかって、無限ループ発動しておく
        // イベントマネージャーを保持しておこうと思う。
        Nan::AsyncQueueWorker(new AsyncEventWorker(conn->eventManager_));
    }
    /**
     * イベントを追加しておく。
     * @param name
     * @param callback
     */
    static NAN_METHOD(AddEventListener) {
        NetConnection *conn = Nan::ObjectWrap::Unwrap<NetConnection>(info.Holder());
        conn->eventManager_->addEventListener(conn, info, NULL);
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    // これ・・・複数同時にくる可能性があるっぽい・・・
    ttLibC_Amf0Object *callObject_; // callbackで必要になる。amf0Objectデータ
};

// netStreamの動作
class NetStream : public RtmpClientBase {
public:
    static void moduleInit(Local<Object> target) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("NetStream").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "play", Play);
        SetPrototypeMethod(tpl, "setBufferLength", SetBufferLength);
        SetPrototypeMethod(tpl, "setFrameListener", SetFrameListener);
        SetPrototypeMethod(tpl, "publish", Publish);
        SetPrototypeMethod(tpl, "queueFrame", QueueFrame);
        SetPrototypeMethod(tpl, "addEventListener", AddEventListener);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

        Nan::Set(
            target,
            Nan::New("NetStream").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());

    }
    void doCallback(rtmpEventList_t *event) {
        if(!isCalled_) {
            // calledフラグが立ってない場合は今回呼ばれるべき処理ではない。
            return;
        }
        if(strcmp(event->name, "onStatusEvent") == 0 && callObjectList_->size != 0) {
            while(true) {
                ttLibC_Amf0Object *obj = (ttLibC_Amf0Object *)ttLibC_StlList_refFirst(callObjectList_);
                if(obj == NULL) {
                    break;
                }
                ttLibC_StlList_remove(callObjectList_, obj);
                Local<Object> jsObject = Nan::New<Object>();
                Nan::Set(jsObject, Nan::New("info").ToLocalChecked(), makeJsObject_from_ttLibCAmf0(obj));
                ttLibC_Amf0_close(&obj);
                Local<Value> args[] = {
                    jsObject
                };
                event->callback->Call(1, args);
            }
            isCalled_ = false;
        }
        else if(strcmp(event->name, "onFrame") == 0 && receiveFrameList_->size != 0) {
            while(true) {
                ttLibC_Frame *frame = (ttLibC_Frame *)ttLibC_StlList_refFirst(receiveFrameList_);
                if(frame == NULL) {
                    break;
                }
                ttLibC_StlList_remove(receiveFrameList_, frame);
                Local<Object> jsFrame = Nan::New<Object>();
                if(!setupJsFrameObject(
                        jsFrame,
                        (ttLibC_Frame *)frame)) {
                    Local<Value> args[] = {
                        Nan::New("Jsオブジェクト作成失敗").ToLocalChecked(),
                        Nan::Null()};
                    event->callback->Call(2, args);
                }
                else {
                    Local<Value> args[] = {
                        Nan::Null(),
                        jsFrame};
                    event->callback->Call(2, args);
                }
                ttLibC_Frame_close(&frame);
            }
            isCalled_ = false;
        }
        else {
//            isCalled_ = false;
        }
    }
private:
    explicit NetStream(ttLibC_RtmpConnection *conn) {
        stream_ = ttLibC_RtmpStream_make(conn);
        ttLibC_RtmpStream_addEventListener(stream_, _onStatusEventCallback, this);
        ttLibC_RtmpStream_addFrameListener(stream_, _onFrameCallback, this);
        isCalled_ = false;
        callObjectList_ = ttLibC_StlList_make();
        receiveFrameList_ = ttLibC_StlList_make();
        frameManager_ = new JsFrameManager();
    }
    ~NetStream() {
        ttLibC_StlList_forEach(callObjectList_, clearAllAmf0Callback, NULL);
        ttLibC_StlList_close(&callObjectList_);
        ttLibC_StlList_forEach(receiveFrameList_, clearAllFrameCallback, NULL);
        ttLibC_StlList_close(&receiveFrameList_);
        ttLibC_RtmpStream_close(&stream_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            NetConnection *conn = Nan::ObjectWrap::Unwrap<NetConnection>(info[0]->ToObject());
            NetStream *stream = new NetStream(conn->conn_);
            stream->eventManager_ = conn->eventManager_; // イベントマネージャーをコピーしておく。
            stream->Wrap(info.This());
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
    static bool clearAllFrameCallback(void *ptr, void *item) {
        ttLibC_Frame *frame = (ttLibC_Frame *)item;
        ttLibC_Frame_close(&frame);
        return true;
    }
    static bool _onFrameCallback(
            void *ptr,
            ttLibC_Frame *frame) {
        // 取得フレームをlistにcloneで保存しなければならないか・・・
        // eventManagerに通知して、jsの世界のcallbackを実行する必要がある・・・と・・・
        NetStream *stream = (NetStream *)ptr;
        stream->isCalled_ = true;
        stream->eventManager_->isCalled_ = true;
        ttLibC_StlList_addLast(stream->receiveFrameList_, ttLibC_Frame_clone(NULL, frame));
        return true;
    }
    static bool _onStatusEventCallback(
            void *ptr,
            ttLibC_Amf0Object *obj) {
        NetStream *stream = (NetStream *)ptr;
        stream->isCalled_ = true;
        stream->eventManager_->isCalled_ = true;
        ttLibC_StlList_addLast(stream->callObjectList_, ttLibC_Amf0_clone(obj));
        return true;
    }
    static NAN_METHOD(Play) {
        if(info.Length() < 1) {
            puts("パラメーターは1つ以上");
            info.GetReturnValue().Set(false);
            return;
        }
        if(!info[0]->IsString()) {
            puts("パラメーターは文字列であるべき");
            info.GetReturnValue().Set(false);
            return;
        }
        bool hasVideo = true;
        bool hasAudio = true;
        if(info.Length() > 1) {
            if(info[1]->IsBoolean()) {
                hasVideo = info[1]->IsTrue();
            }
        }
        if(info.Length() > 2) {
            if(info[2]->IsBoolean()) {
                hasAudio = info[2]->IsTrue();
            }
        }
        NetStream* stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
        // playを実行する。
        v8::String::Utf8Value str(info[0]->ToString());
        ttLibC_RtmpStream_play(stream->stream_, (const char *)(*str), hasVideo, hasAudio);
        info.GetReturnValue().Set(true);
    }
    static NAN_METHOD(SetBufferLength) {
        if(info.Length() != 1) {
            puts("パラメーターは1つ");
            info.GetReturnValue().Set(false);
            return;
        }
        if(!info[0]->IsNumber()) {
            puts("パラメーターは数値であるべき");
            info.GetReturnValue().Set(false);
            return;
        }
        NetStream* stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
        int value = (int)info[0]->NumberValue();
        ttLibC_RtmpStream_setBufferLength(stream->stream_, value * 1000);
        info.GetReturnValue().Set(true);
    }
    static NAN_METHOD(SetFrameListener) {
        // 面倒だ・・・
        // addEventListenerと同じ処理にしておけばいいか・・・
        // 文字列の部分が存在しないので、勝手な文字列を捏造しておこうと思う。
        // またinfoから取得することができないので、ちょっと手を加えておかないといけない。
        NetStream *stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
        stream->eventManager_->addEventListener(stream, info, "onFrame");
    }
    static NAN_METHOD(Publish) {
        if(info.Length() != 1) {
            puts("パラメーターは1つ");
            info.GetReturnValue().Set(false);
            return;
        }
        if(!info[0]->IsString()) {
            puts("パラメーターは文字列であるべき");
            info.GetReturnValue().Set(false);
            return;
        }
        NetStream* stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
        // publishを実行する。
        v8::String::Utf8Value str(info[0]->ToString());
        ttLibC_RtmpStream_publish(stream->stream_, (const char *)(*str));
        info.GetReturnValue().Set(true);
    }
    static NAN_METHOD(QueueFrame) {
        if(info.Length() != 1) {
            puts("パラメーターはフレーム1つである必要があります。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!info[0]->IsObject()) {
            puts("1st argはframeオブジェクトでないとだめです。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        NetStream* stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
        ttLibC_Frame *frame = stream->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元できなかった。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!ttLibC_RtmpStream_addFrame(
                stream->stream_,
                frame)) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        info.GetReturnValue().Set(Nan::New(true));
    }
    /**
     * イベントを追加しておく。
     * @param name
     * @param callback
     */
    static NAN_METHOD(AddEventListener) {
        NetStream *stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
        stream->eventManager_->addEventListener(stream, info, NULL);
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    EventManager *eventManager_;
    ttLibC_RtmpStream *stream_;
    // callbackで受け取ったamf0オブジェクトリスト
    ttLibC_StlList *callObjectList_;
    // play時にうけとったframeリスト
    ttLibC_StlList *receiveFrameList_;
    // フレームを扱うためのマネージャー
    JsFrameManager *frameManager_;
};

#endif

static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
    Local<FunctionTemplate> tpl;
    NetConnection::moduleInit(target);
    NetStream::moduleInit(target);
#endif
}

NODE_MODULE(rtmpClient, Init);

// 以下諸々の補助的な関数とか。
static Local<Value> makeJsObject_from_ttLibCAmf0(ttLibC_Amf0Object *src_obj) {
    switch(src_obj->type) {
    case amf0Type_Number:
        return Nan::New(*((double *)src_obj->object));
    case amf0Type_Boolean:
        return Nan::New((bool)*((uint8_t *)src_obj->object) == 1);
    case amf0Type_String:
        return Nan::New((const char *)src_obj->object).ToLocalChecked();
    case amf0Type_Object:
        {
            Local<Object> object = Nan::New<Object>();
            ttLibC_Amf0MapObject *src_lists = (ttLibC_Amf0MapObject *)src_obj->object;
            for(int i = 0;src_lists[i].key != NULL && src_lists[i].amf0_obj != NULL;++ i) {
                Nan::Set(object, Nan::New(src_lists[i].key).ToLocalChecked(), makeJsObject_from_ttLibCAmf0(src_lists[i].amf0_obj));
            }
            return object;
        }
        break;
    case amf0Type_MovieClip:
        break;
    case amf0Type_Null:
        break;
    case amf0Type_Undefined:
    case amf0Type_Reference:
        break;
    case amf0Type_Map:
        {
            Local<Object> object = Nan::New<Object>();
            ttLibC_Amf0MapObject *src_lists = (ttLibC_Amf0MapObject *)src_obj->object;
            for(int i = 0;src_lists[i].key != NULL && src_lists[i].amf0_obj != NULL;++ i) {
                Nan::Set(object, Nan::New(src_lists[i].key).ToLocalChecked(), makeJsObject_from_ttLibCAmf0(src_lists[i].amf0_obj));
            }
            return object;
        }
        break;
    case amf0Type_ObjectEnd:
    case amf0Type_Array:
        // これ未実装でいいんかいw
    case amf0Type_Date:
    case amf0Type_LongString:
    case amf0Type_Unsupported:
    case amf0Type_RecordSet:
    case amf0Type_XmlDocument:
    case amf0Type_TypedObject:
    case amf0Type_Amf3Object:
    default:
        break;
    }
    return Nan::New("undefined").ToLocalChecked();
}
