// mp4の読み込みまわりの処理
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>
#include <ttLibC/container/mp4.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class Mp4Reader : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        ttLibC_Allocator_init();
//        FramePassingWorker::Init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("Mp4Reader").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "read", Read);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("Mp4Reader").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
    explicit Mp4Reader() {
        reader_ = ttLibC_Mp4Reader_make();
    }
    ~Mp4Reader() {
        ttLibC_Mp4Reader_close(&reader_);
        ttLibC_Allocator_close();
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            Mp4Reader *reader = new Mp4Reader();
            reader->Wrap(info.This());
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
    static bool frameCallback(void *ptr, ttLibC_Frame *frame) {
        Mp4Reader *reader = (Mp4Reader *)ptr;
        auto callback = new Nan::Callback(reader->callback_.As<Function>());
//        Nan::AsyncQueueWorker(new FramePassingWorker(frame, callback));
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
        return true;
    }
    static bool mp4Callback(void *ptr, ttLibC_Mp4 *mp4) {
        return ttLibC_Mp4_getFrame(
                mp4,
                frameCallback,
                ptr);
    }
    static NAN_METHOD(Read) {
        // 入力はbinaryデータとcallback
        if(info.Length() == 2) {
            Local<Value> binary = info[0];
            if(binary->IsUint8Array()
            || binary->IsArrayBuffer()) {
                uint8_t *data = (uint8_t *)node::Buffer::Data(binary->ToObject());
                size_t data_size = node::Buffer::Length(binary->ToObject());
                Mp4Reader* reader = Nan::ObjectWrap::Unwrap<Mp4Reader>(info.Holder());
                reader->callback_ = info[1];
                if(ttLibC_Mp4Reader_read(
                        reader->reader_,
                        data,
                        data_size,
                        mp4Callback,
                        reader)) {
                    info.GetReturnValue().Set(true);
                    return;
                }
            }
        }
        info.GetReturnValue().Set(false);
    }
    static NAN_METHOD(Dump) {
        ttLibC_Allocator_dump();
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_Mp4Reader *reader_;
    Local<Value> callback_;
};

NODE_MODULE(mp4Reader, Mp4Reader::Init);
