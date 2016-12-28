// mpegtsの書き出し動作
#include <nan.h>
#include "../frame/frame.hpp"
#include "../util/binary.hpp"

#include <ttLibC/allocator.h>
#include <ttLibC/container/mpegts.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class MpegtsWriter : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        ttLibC_Allocator_init();
//        BinaryPassingWorker::Init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("MpegtsWriter").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "write", Write);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("MpegtsWriter").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
    explicit MpegtsWriter(
            ttLibC_Frame_Type *types,
            uint32_t types_num) {
        writer_ = ttLibC_MpegtsWriter_make(types, types_num);
        frameManager_ = new JsFrameManager();
    }
    ~MpegtsWriter() {
        ttLibC_MpegtsWriter_close(&writer_);
        delete frameManager_;
    }
    static NAN_METHOD(New) {
        if(info.IsConstructCall()) {
            ttLibC_Frame_Type *types = new ttLibC_Frame_Type[info.Length()];
            int num = info.Length();
            for(int i = 0;i < num;++ i) {
                String::Utf8Value codec(info[i]->ToString());
                if(strcmp((const char *)*codec, "h264") == 0) {
                    types[i] = frameType_h264;
                }
                else if(strcmp((const char *)*codec, "aac") == 0) {
                    types[i] = frameType_aac;
                }
                else if(strcmp((const char *)*codec, "mp3") == 0) {
                    types[i] = frameType_mp3;
                }
            }
            MpegtsWriter *obj = new MpegtsWriter(types, num);
            obj->Wrap(info.This());
            Nan::Set(info.This(), Nan::New("enableDts").ToLocalChecked(),       Nan::New(false));
            Nan::Set(info.This(), Nan::New("splitType").ToLocalChecked(),       Nan::New(0));
            Nan::Set(info.This(), Nan::New("pts").ToLocalChecked(),             Nan::New((double)obj->writer_->pts));
            Nan::Set(info.This(), Nan::New("timebase").ToLocalChecked(),        Nan::New((uint32_t)obj->writer_->timebase));
            info.GetReturnValue().Set(info.This());

            delete[] types;
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
        MpegtsWriter *writer = (MpegtsWriter *)ptr;
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
        MpegtsWriter* writer = Nan::ObjectWrap::Unwrap<MpegtsWriter>(info.Holder());
        Local<Value> enableDts = Nan::Get(info.Holder(), Nan::New("enableDts").ToLocalChecked()).ToLocalChecked();
        uint32_t mode = containerWriter_keyFrame_split;
        if(enableDts->IsBoolean()) {
            if(enableDts->IsTrue()) {
                mode |= containerWriter_enable_dts;
            }
        }
        Local<Value> splitType = Nan::Get(info.Holder(), Nan::New("splitType").ToLocalChecked()).ToLocalChecked();
        if(splitType->IsNumber()) {
            uint32_t divtype = (uint32_t)splitType->NumberValue();
            mode |= divtype;
        }
        writer->writer_->mode = mode;
        writer->callback_ = info[1];
        // フレームを取得
        ttLibC_Frame *frame = writer->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元することができてない。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!ttLibC_MpegtsWriter_write(
                writer->writer_,
                frame,
                writeCallback,
                writer)) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        Nan::Set(info.This(), Nan::New("pts").ToLocalChecked(), Nan::New((double)writer->writer_->pts));
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(Dump) {
        ttLibC_Allocator_dump();
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_MpegtsWriter *writer_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
};

NODE_MODULE(mpegtsWriter, MpegtsWriter::Init);
