// flvの動作まわりの処理
#include <nan.h>

#include "../frame/frame.hpp"

#include <ttLibC/container/flv.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class FlvReader : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("FlvReader").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "read", Read);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("FlvReader").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
    explicit FlvReader() {
        reader_ = ttLibC_FlvReader_make();
    }
    ~FlvReader() {
        ttLibC_FlvReader_close(&reader_);
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            FlvReader *reader = new FlvReader();
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
        FlvReader *reader = (FlvReader *)ptr;
        // async使わないようにしてみた。
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
    static bool flvCallback(void *ptr, ttLibC_Flv *flv) {
        return ttLibC_Flv_getFrame(
                flv,
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
                FlvReader* reader = Nan::ObjectWrap::Unwrap<FlvReader>(info.Holder());
                reader->callback_ = info[1];
                if(ttLibC_FlvReader_read(
                        reader->reader_,
                        data,
                        data_size,
                        flvCallback,
                        reader)) {
                    // 読み込み成功した。
                    info.GetReturnValue().Set(true);
                    return;
                }
            }
        }
        info.GetReturnValue().Set(false);
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    ttLibC_FlvReader *reader_;
    Local<Value> callback_;
};

NODE_MODULE(flvReader, FlvReader::Init);
