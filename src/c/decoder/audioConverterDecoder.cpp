// aacやmp3のデータをpcms16にする。(OSXのaudioConverterを利用する)
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>

#ifdef __ENABLE__
#   include <ttLibC/decoder/audioConverterDecoder.h>
#endif

#include <ttLibC/frame/frame.h>
#include <ttLibC/frame/audio/audio.h>
#include <ttLibC/frame/audio/aac.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <pthread.h>
#include <map>
#include <list>

using namespace v8;

#ifdef __ENABLE__
typedef struct forMutex_t {
    std::list<ttLibC_Frame *> *frame_list;
    pthread_mutex_t mutex;
} forMutex_t;
static std::map<uint32_t, forMutex_t *> *mutex_map_;

class AsyncAcDecodeWorker : public Nan::AsyncWorker {
public:
    AsyncAcDecodeWorker(
            ttLibC_AcDecoder *decoder,
            ttLibC_Audio *audio,
            Nan::Callback *callback) :
                Nan::AsyncWorker(callback) {
        id_ = audio->inherit_super.id;
        std::map<uint32_t, forMutex_t *>::iterator iter = mutex_map_->find(id_);
        forMutex_t *mtx;
        if(iter == mutex_map_->end()) {
            // 初アクセスなので、セットアップしておかなければならない。
            mtx = (forMutex_t *)ttLibC_malloc(sizeof(forMutex_t));
            pthread_mutex_init(&mtx->mutex, NULL);
            mtx->frame_list = new std::list<ttLibC_Frame *>();
            // 次回以降のために、登録しておく。
            mutex_map_->insert(std::pair<uint32_t, forMutex_t *>(id_, mtx));
        }
        else {
            mtx = (forMutex_t *)iter->second;
        }
        mutex_ = mtx;
        // semaphoreのセットアップしておく。
        if((semid_ = semget(IPC_PRIVATE, 2, 0600)) == -1) {
            puts("semget失敗");
            return;
        }
        val_ = 0;
        ctl_arg_.array = &val_;
        if(semctl(semid_, 0, SETALL, ctl_arg_) == -1) {
            puts("semctlで初期化失敗");
            return;
        }
        // デコード実施
        if(!ttLibC_AcDecoder_decode(
                decoder,
                audio,
                decodeCallback,
                this)) {
            puts("変換エラー発生");
        }
        if(audio->inherit_super.type == frameType_aac) {
            ttLibC_Aac *aac = (ttLibC_Aac *)audio;
            if(aac->type == AacType_dsi) {
                mysemop(1);// unlockしておく。
            }
        }
    }
    static bool decodeCallback(void *ptr, ttLibC_PcmS16 *pcm) {
        AsyncAcDecodeWorker *worker = (AsyncAcDecodeWorker *)ptr;
        worker->mysemop(1);
        int r = pthread_mutex_lock(&worker->mutex_->mutex);
        if(r != 0) {
            puts("failed lock.");
            return false;
        }
        // ここでlistにデータを突っ込む mutexのlockあり
        ttLibC_Frame *frame = (ttLibC_Frame *)ttLibC_PcmS16_clone(NULL, pcm);
        worker->mutex_->frame_list->push_back(frame);
        r = pthread_mutex_unlock(&worker->mutex_->mutex);
        if(r != 0) {
            puts("failed unlock.");
            return false;
        }
        return true;
    }
    void Execute() {
        mysemop(-1); // lockしておく。先にcallbackがきている場合は、スルーされる。
    }
    void HandleOKCallback() {
        // セマフォいらないので、クリアしとく。
        if(semctl(semid_, 0, IPC_RMID, ctl_arg_) == -1) {
            puts("semctlで解放失敗");
        }
        // ここでlistからデータを取り出す。mutexのlockあり
        int r = pthread_mutex_lock(&mutex_->mutex);
        if(r != 0) {
            puts("failed lock.");
            return;
        }
        ttLibC_Frame *frame = NULL;
        if(mutex_->frame_list->size() != 0) {
            frame = mutex_->frame_list->front();
        }
        if(frame != NULL) {
            mutex_->frame_list->remove(frame);
            // フレームデータをcallbackで応答する。
            Local<Object> jsFrame = Nan::New<Object>();
            if(!setupJsFrameObject(
                    jsFrame,
                    frame)) {
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
            ttLibC_Frame_close(&frame);
        }
        // ここでlistにデータを突っ込む mutexのlockあり
        r = pthread_mutex_unlock(&mutex_->mutex);
        if(r != 0) {
            puts("failed unlock.");
            return;
        }
    }
private:
    // -1でlock 1でunlock
    void mysemop(int sem_op) {
        struct sembuf sops;
        sops.sem_num = 0;
        sops.sem_op = sem_op;
        sops.sem_flg = 0;
        if(semop(semid_, &sops, 1) == -1) {
            puts("mysemop失敗");
        }
    }
    int id_; // 動作対象のフレームID
    // semaphoreで利用するものたち
    int semid_;
    union semun {
        int val;
        struct semid_ds *buf;
        uint16_t *array;
    } ctl_arg_;
    uint16_t val_;

    // mutexで利用するものたち。
    forMutex_t *mutex_; // この動作で必要となる、mutexデータ
};

class Initializer {
public:
    Initializer() {
        mutex_map_ = new std::map<uint32_t, forMutex_t *>();
    }
    ~Initializer() {
        // ここでmutex_map上のオブジェクトをttLibC_freeしておかないとメモリーリークになる。
        // まぁ、プログラムが終わるときに、解放すべきデータということなので、やらなくてもOSが解放すると思うけど。
    }
};
Initializer init;
#endif

class AudioConverterDecoder : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
#ifdef __ENABLE__
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("AudioConverterDecoder").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "decode", Decode);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("AudioConverterDecoder").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
#endif
    }
#ifdef __ENABLE__
private:
    explicit AudioConverterDecoder(
            uint32_t sample_rate,
            uint32_t channel_num,
            ttLibC_Frame_Type frame_type) {
        decoder_ = ttLibC_AcDecoder_make(sample_rate, channel_num, frame_type);
        frameManager_ = new JsFrameManager();
    }
    ~AudioConverterDecoder() {
        // TODO このタイミングで対応している、mutexを解放しないとまずくね？
        ttLibC_AcDecoder_close(&decoder_);
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
                if(strcmp((const char *)*codec, "aac") == 0) {
                    type = frameType_aac;
                }
                else if(strcmp((const char *)*codec, "mp3") == 0) {
                    type = frameType_mp3;
                }
                AudioConverterDecoder *decoder = new AudioConverterDecoder(
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
    static bool decodeCallback(void *ptr, ttLibC_PcmS16 *pcm) {
        AudioConverterDecoder *decoder = (AudioConverterDecoder *)ptr;
        auto callback = new Nan::Callback(decoder->callback_.As<Function>());
        Nan::AsyncQueueWorker(new FramePassingWorker((ttLibC_Frame *)pcm, callback));
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
        AudioConverterDecoder *decoder = Nan::ObjectWrap::Unwrap<AudioConverterDecoder>(info.Holder());
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
        auto callback = new Nan::Callback(info[1].As<Function>());
        Nan::AsyncQueueWorker(new AsyncAcDecodeWorker(decoder->decoder_, (ttLibC_Audio *)frame, callback));
/*        if(!ttLibC_AcDecoder_decode(
                decoder->decoder_,
                (ttLibC_Audio *)frame,
                decodeCallback,
                decoder)) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }*/
        info.GetReturnValue().Set(Nan::New(true));
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_AcDecoder *decoder_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
#endif
};

NODE_MODULE(audioConverterDecoder, AudioConverterDecoder::Init);