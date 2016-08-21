// flvの書き出し動作
#include <nan.h>
#include "../frame/frame.hpp"
#include "../util/binary.hpp"

#include <ttLibC/allocator.h>
#include <ttLibC/container/flv.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class FlvWriter : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        ttLibC_Allocator_init();
//        BinaryPassingWorker::Init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("FlvWriter").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "write", Write);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("FlvWriter").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
    explicit FlvWriter(
            ttLibC_Frame_Type video_type,
            ttLibC_Frame_Type audio_type) {
        writer_ = ttLibC_FlvWriter_make(video_type, audio_type);
        frameManager_ = new JsFrameManager();
    }
    ~FlvWriter() {
        ttLibC_FlvWriter_close(&writer_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            String::Utf8Value video(info[0]->ToString());
            String::Utf8Value audio(info[1]->ToString());
            ttLibC_Frame_Type video_type = frameType_unknown;
            ttLibC_Frame_Type audio_type = frameType_unknown;
            if(strcmp((const char *)*video, "h264") == 0) {
                video_type = frameType_h264;
            }
            else if(strcmp((const char *)*video, "flv1") == 0) {
                video_type = frameType_flv1;
            }
            if(strcmp((const char *)*audio, "aac") == 0) {
                audio_type = frameType_aac;
            }
            else if(strcmp((const char *)*audio, "mp3") == 0) {
                audio_type = frameType_mp3;
            }
            FlvWriter *obj = new FlvWriter(video_type, audio_type);
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
    static bool writeCallback(void *ptr, void *data, size_t data_size) {
        FlvWriter *writer = (FlvWriter *)ptr;
        auto callback = new Nan::Callback(writer->callback_.As<Function>());
//        Nan::AsyncQueueWorker(new BinaryPassingWorker((uint8_t *)data, data_size, callback));
        Local<Object> binary = Nan::CopyBuffer((char *)data, data_size).ToLocalChecked();
        Local<Value> args[] = {
            Nan::Null(),
            binary};
        callback->Call(2, args);
        return true;
    }
    static NAN_METHOD(Write) {
        // ここで書き込みを実施する。
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
        FlvWriter* writer = Nan::ObjectWrap::Unwrap<FlvWriter>(info.Holder());
        writer->callback_ = info[1];
        // フレームを取得
        ttLibC_Frame *frame = writer->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元することができてない。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!ttLibC_FlvWriter_write(
                writer->writer_,
                frame,
                writeCallback,
                writer)) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(Dump) {
        ttLibC_Allocator_dump();
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_FlvWriter *writer_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
};

NODE_MODULE(flvWriter, FlvWriter::Init);