// サーバーってつくるのかな？とりあえずrtmpのclient動作つくっておこう。
// publishとplayで別にすべきか？
// まぁいいけどw
// 読み込むという意味ではreaderが近いか？

/**
 * とりあえずasににせておこう。
 var nc = new NetConnection();
 nc.addEventListener("onStatusEvent", function(event:NetStatusEvent):void {
     event.info.code;
 });
 // こんな感じだったっか。
 nc.onStatus = function(info) {
   // ここでnetStatusイベント
   if(info.code == "NetConnection.Connect.Success") {
     var ns = new NetStream(nc);
     ns.publish("test");
     ns.play("test");
     これだけでいいや。
   }
 }
 nc.connect("rtmp://192.168.11.11/live"); // 適当なアドレス
 */
// せっかくなので、それっぽくつくってみよう。
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

// listenerのデータを保持するための構造体
typedef struct rtmpEventList_t{
    char name[256];
    Nan::Callback *callback;
} rtmpEventList_t;

class AsyncRtmpWorker : public Nan::AsyncWorker {
public:
    AsyncRtmpWorker(
        Nan::Callback *callback,
        void *client) :
            Nan::AsyncWorker(callback), client_(client) {
    }
    void Execute();
    void HandleOKCallback();
private:
    void *client_;
};

static Local<Value> makeJsObject_from_ttLibCAmf0(ttLibC_Amf0Object *src_obj);

class RtmpClient : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        ttLibC_Allocator_init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(NewConn);
        tpl->SetClassName(Nan::New("NetConnection").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "connect", Connect);
        SetPrototypeMethod(tpl, "addEventListener", AddEventListener);
        SetPrototypeMethod(tpl, "removeEventListener", RemoveEventListener);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

        Nan::Set(
            target,
            Nan::New("NetConnection").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());

        tpl = Nan::New<FunctionTemplate>(NewStream);
        tpl->SetClassName(Nan::New("NetStream").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "publish", Publish);
        SetPrototypeMethod(tpl, "queueFrame", QueueFrame);
        SetPrototypeMethod(tpl, "play", Play);
        SetPrototypeMethod(tpl, "setBufferLength", SetBufferLength);
        SetPrototypeMethod(tpl, "addFrameListener", AddFrameListener);
        SetPrototypeMethod(tpl, "addEventListener", AddEventListener);
        SetPrototypeMethod(tpl, "removeEventListener", RemoveEventListener);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

        Nan::Set(
            target,
            Nan::New("NetStream").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
    bool doUpdate() {
        while(true) {
            if(!ttLibC_RtmpConnection_update(conn_, 10000)) {
                break;
            }
            // ここのcallEventはこのRtmpClientのも必要だけど、配下のnetStreamのRtmpClientのも必要になりそう。
            // 配下のnetStreamはいくつできるか不明なので、そっちも考慮が必要。
            if(callEvent_) {
                return true;
            }

        }
        return false;
    }
    bool callEvent() {
        if(callEvent_) {
            rtmpEventList_t *list = (rtmpEventList_t *)ttLibC_StlList_refFirst(eventList_);
            // amf0ObjectをjsObject化する。
            Local<Object> jsObject = Nan::New<Object>();
            Nan::Set(jsObject, Nan::New("info").ToLocalChecked(), makeJsObject_from_ttLibCAmf0(callObject_));
            Local<Value> args[] = {
                jsObject
            };
            list->callback->Call(1, args);
            callEvent_ = false;
            return true;
        }
        return false;
    }
private:
    static bool _onStatusEventCallback(
            void *ptr,
            ttLibC_Amf0Object *obj) {
        RtmpClient *client = (RtmpClient *)ptr;
        // 結局cloneしてから解放させられるなら、ここでjsのobjectにしても同じか・・・
        // やっちゃおう。
        // 呼ばれるのがjsの世界ではないんだが・・・そこだけちょっと心配
        // これやっぱり無理なのか・・・
        // callbackをつかって、設定していかなければならないのか・・・
        // とりあえず中身を解析しないとな・・・
        client->callEvent_ = true;
        // 前のobjectが残っていたらメモリーをクリアしておく。
        ttLibC_Amf0_close(&client->callObject_);
        // データをクローンしておく。
        client->callObject_ = ttLibC_Amf0_clone(obj);
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
    explicit RtmpClient(ttLibC_RtmpConnection *conn) {
        eventList_ = ttLibC_StlList_make();
        if(conn == NULL) {
            conn_ = ttLibC_RtmpConnection_make();
            stream_ = NULL;
            // リストをこの段階でつくっておく
            streamRtmpClientList_ = ttLibC_StlList_make();
        }
        else {
            streamRtmpClientList_ = null;
            conn_ = NULL;
            stream_ = ttLibC_RtmpStream_make(conn);
            ttLibC_RtmpStream_addEventListener(stream_, _onStatusEventCallback, this);
        }
        callEvent_ = false;
        callObject_ = NULL;
    }
    ~RtmpClient() {
        ttLibC_StlList_forEach(eventList_, clearAllEventCallback, NULL);
        ttLibC_StlList_close(&eventList_);
        // 全体の動作として、先にstreamが閉じられなければならない。
        // これが少々心配
        // 一応Local<Value>をもたせることで解放があとになるように頑張ってみたけど・・・
        ttLibC_RtmpStream_close(&stream_);
        ttLibC_RtmpConnection_close(&conn_);
    }
    static NAN_METHOD(NewConn) {
        if(info.IsConstructCall()) {
            RtmpClient *client = new RtmpClient(NULL);
            client->Wrap(info.This());
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
    static NAN_METHOD(Connect) {
        // コネクトは重要、超重要
        // まずパラメーターが１つ必要。
        // 基本string
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
        RtmpClient* client = Nan::ObjectWrap::Unwrap<RtmpClient>(info.Holder());
        if(client->conn_ == NULL) {
            puts("stream側でconnectを呼ぶことはできません。まぁここにくることはないだろうけど");
            info.GetReturnValue().Set(false);
            return;
        }
        // 接続する前にeventListenerを設置しなければならない。
        ttLibC_RtmpConnection_addEventListener(client->conn_, _onStatusEventCallback, client);
        v8::String::Utf8Value str(info[0]->ToString());
        ttLibC_RtmpConnection_connect(client->conn_, (const char *)(*str));
        // このあと無限ループに入る必要がある。
        Nan::AsyncQueueWorker(new AsyncRtmpWorker(NULL, client));
        info.GetReturnValue().Set(true);
    }

    static NAN_METHOD(NewStream) {
        if(info.IsConstructCall()) {
            if(info.Length() != 1) {
                puts("パラメーターとして、connectionが必要です。");
            }
            else {
                // これが大丈夫なのか、ちょっと心配。
                RtmpClient *connection = Nan::ObjectWrap::Unwrap<RtmpClient>(info[0]->ToObject());
                RtmpClient *client = new RtmpClient(connection->conn_);
                client->connRtmpClient_ = info[0];
                ttLibC_StlList_addLast(
                        connection->streamRtmpClientList_,
                        client
                );
                client->Wrap(info.This());
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
    static NAN_METHOD(Play) {
        info.GetReturnValue().Set(false);
    }
    static NAN_METHOD(Publish) {
        info.GetReturnValue().Set(false);
    }
    static NAN_METHOD(SetBufferLength) {
        puts("buffer Lengthを指定する。");
        if(info.Length() != 1) {
            puts("パラメーターは1つ");
            info.GetReturnValue().Set(false);
            return;
        }
        if(info[0]->IsNumber()) {
            puts("パラメーターは数値であるべき");
            info.GetReturnValue().Set(false);
            return;
        }
        RtmpClient* client = Nan::ObjectWrap::Unwrap<RtmpClient>(info.Holder());
        if(client->stream_ == NULL) {
            puts("NetStreamでcallしてください。");
            info.GetReturnValue().Set(false);
            return;
        }
        int value = (int)info[0]->NumberValue();
        ttLibC_RtmpStream_setBufferLength(client->stream_, value * 1000);
        info.GetReturnValue().Set(true);
    }
    static NAN_METHOD(QueueFrame) {
        info.GetReturnValue().Set(false);
    }
    static NAN_METHOD(AddFrameListener) {
        // フレームが生成されたときに受け取る動作
        info.GetReturnValue().Set(false);
    }

    static NAN_METHOD(Dump) {
        ttLibC_Allocator_dump();
    }
    static NAN_METHOD(AddEventListener) {
        if(info.Length() < 2) {
            info.GetReturnValue().Set(false);
            return;
        }
        // 1つ目は文字列であることを期待する。
        // 2つ目はcallbackであることを期待する。
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
        RtmpClient* client = Nan::ObjectWrap::Unwrap<RtmpClient>(info.Holder());
        // あとはこのデータを登録する。
        rtmpEventList_t *event = (rtmpEventList_t *)ttLibC_malloc(sizeof(rtmpEventList_t));
        v8::String::Utf8Value str(info[0]->ToString());
        sprintf(event->name, (const char *)(*str));
        auto callback = new Nan::Callback(info[1].As<Function>());
        event->callback = callback;
        ttLibC_StlList_addLast(client->eventList_, event);
        puts("eventListenerを追加することできました。");
        printf("アドレス%d", event);
    }
    static NAN_METHOD(RemoveEventListener) {
        // これ・・・remove実装できなくね？
/*        if(info.Length() < 2) {
            info.GetReturnValue().Set(false);
            return;
        }
        // 1つ目は文字列であることを期待する。
        // 2つ目はcallbackであることを期待する。
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
        RtmpClient* client = Nan::ObjectWrap::Unwrap<RtmpClient>(info.Holder());*/
        // listの中から同じ名前で同じobjectになっているものを見つけて撤去しないとだめ。
        // とりあえず動作に重要ではないので、あとでやる。
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    // connectionを扱うか、streamを扱うかはコンストラクタ次第
    ttLibC_RtmpConnection *conn_;
    ttLibC_RtmpStream *stream_;
    // 親になるrtmpClientを保持する理由はないのか？(たぶんなさそう)
    // 逆に親側に子になる、rtmpClientを保持しておかないとまずそう。
    Local<Value> connRtmpClient_; // 削除するときに、必要になるか？
    ttLibC_StlList *streamRtmpClientList_;

    bool callEvent_; // callEventが発生したら、true
    ttLibC_StlList *eventList_; // 処理すべきlistenerリスト
    ttLibC_Amf0Object *callObject_; // callbackで利用するamf0データ
};

NODE_MODULE(rtmpClient, RtmpClient::Init);

void AsyncRtmpWorker::Execute() {
    puts("executeします。");
    RtmpClient *client = (RtmpClient *)client_;
    client->doUpdate();
    // このasyncWorkerで見てるのはnetConnectionだけになるわけか・・・
    // ここにnetStreamも追加しておいて、netStreamの場合でも、処理を中断して、Javascript側の空間に復帰できるようにしなければならないわけか・・・
}

void AsyncRtmpWorker::HandleOKCallback() {
    puts("handle OKが呼ばれた。おわり。");
    RtmpClient *client = (RtmpClient *)client_;
    // これはいけるのか・・・なるほど・・・
    // 再度queueを発行するのはOKなわけか・・・なるなる
    if(client->callEvent()) {
        Nan::AsyncQueueWorker(new AsyncRtmpWorker(NULL, client));
    }
}

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
