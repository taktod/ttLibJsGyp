// mp4の書き出し動作
#include <nan.h>
#include "../frame/frame.hpp"
#include "../util/binary.hpp"

#include <ttLibC/allocator.h>
#include <ttLibC/container/mp4.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class Mp4Writer : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        ttLibC_Allocator_init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("Mp4Writer").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "write", Write);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("Mp4Writer").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
    explicit Mp4Writer(
            ttLibC_Frame_Type *types,
            uint32_t types_num) {
        writer_ = ttLibC_Mp4Writer_make(types, types_num);
        frameManager_ = new JsFrameManager();
    }
    ~Mp4Writer() {
        ttLibC_Mp4Writer_close(&writer_);
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
            }
            Mp4Writer *obj = new Mp4Writer(types, num);
            obj->Wrap(info.This());
            Nan::Set(info.This(), Nan::New("enableDts").ToLocalChecked(),      Nan::New(false));
            Nan::Set(info.This(), Nan::New("splitType").ToLocalChecked(),      Nan::New(0));
            Nan::Set(info.This(), Nan::New("splitTypeKey").ToLocalChecked(),   Nan::New(0));
            Nan::Set(info.This(), Nan::New("splitTypeInner").ToLocalChecked(), Nan::New(1));
            Nan::Set(info.This(), Nan::New("splitTypeAll").ToLocalChecked(),   Nan::New(2));
            Nan::Set(info.This(), Nan::New("pts").ToLocalChecked(),            Nan::New(0));
            Nan::Set(info.This(), Nan::New("timebase").ToLocalChecked(),       Nan::New(1000));
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
        Mp4Writer *writer = (Mp4Writer *)ptr;
        auto callback = new Nan::Callback(writer->callback_.As<Function>());
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
        Mp4Writer* writer = Nan::ObjectWrap::Unwrap<Mp4Writer>(info.Holder());
        Local<Value> enableDts = Nan::Get(info.Holder(), Nan::New("enableDts").ToLocalChecked()).ToLocalChecked();
        uint32_t mode = 0x00;
        if(enableDts->IsBoolean()) {
            if(enableDts->IsTrue()) {
                mode |= containerWriter_enable_dts;
            }
        }
        Local<Value> splitType = Nan::Get(info.Holder(), Nan::New("splitType").ToLocalChecked()).ToLocalChecked();
        if(splitType->IsNumber()) {
            switch((uint32_t)splitType->NumberValue()) {
            case 0:
            default:
                mode |= containerWriter_keyFrame_division;
                break;
            case 1:
                mode |= containerWriter_innerFrame_division;
                break;
            case 2:
                mode |= containerWriter_allFrame_division;
                break;
            }
        }
        writer->writer_->inherit_super.mode = mode;
        writer->callback_ = info[1];
        // フレームを取得
        ttLibC_Frame *frame = writer->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元することができてない。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
//        printf("pts check:%llu\n", frame->pts);
        if(!ttLibC_Mp4Writer_write(
                writer->writer_,
                frame,
                writeCallback,
                writer)) {
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        Nan::Set(info.This(), Nan::New("pts").ToLocalChecked(), Nan::New((double)writer->writer_->inherit_super.pts));
        info.GetReturnValue().Set(Nan::New(true));
    }
    static NAN_METHOD(Dump) {
        ttLibC_Allocator_dump();
    }
    static inline Nan::Persistent<Function> & constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }
    ttLibC_Mp4Writer *writer_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
};

NODE_MODULE(mp4Writer, Mp4Writer::Init);