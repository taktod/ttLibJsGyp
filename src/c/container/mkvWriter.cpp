// mkv書き出し
#include <nan.h>
#include "../frame/frame.hpp"

#include <ttLibC/allocator.h>
#include <ttLibC/container/mkv.h>
#include <ttLibC/frame/frame.h>
#include <stdlib.h>

using namespace v8;

class MkvWriter : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        ttLibC_Allocator_init();
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("MkvWriter").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "write", Write);
        SetPrototypeMethod(tpl, "dump", Dump);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(
            target,
            Nan::New("MkvWriter").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
    explicit MkvWriter(
            ttLibC_Frame_Type *types,
            uint32_t types_num) {
        writer_ = ttLibC_MkvWriter_make(types, types_num);
        frameManager_ = new JsFrameManager();
    }
    ~MkvWriter() {
        ttLibC_MkvWriter_close(&writer_);
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
                else if(strcmp((const char *)*codec, "h265") == 0) {
                    types[i] = frameType_h265;
                }
                else if(strcmp((const char *)*codec, "jpeg") == 0) {
                    types[i] = frameType_jpeg;
                }
                else if(strcmp((const char *)*codec, "theora") == 0) {
                    types[i] = frameType_theora;
                }
                else if(strcmp((const char *)*codec, "vp8") == 0) {
                    types[i] = frameType_vp8;
                }
                else if(strcmp((const char *)*codec, "vp9") == 0) {
                    types[i] = frameType_vp9;
                }
                else if(strcmp((const char *)*codec, "aac") == 0) {
                    types[i] = frameType_aac;
                }
                else if(strcmp((const char *)*codec, "adpcmimawav") == 0) {
                    types[i] = frameType_adpcm_ima_wav;
                }
                else if(strcmp((const char *)*codec, "mp3") == 0) {
                    types[i] = frameType_mp3;
                }
                else if(strcmp((const char *)*codec, "opus") == 0) {
                    types[i] = frameType_opus;
                }
                else if(strcmp((const char *)*codec, "speex") == 0) {
                    types[i] = frameType_speex;
                }
                else if(strcmp((const char *)*codec, "vorbis") == 0) {
                    types[i] = frameType_vorbis;
                }
            }
            MkvWriter *obj = new MkvWriter(types, num);
            obj->Wrap(info.This());
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
        MkvWriter *writer = (MkvWriter *)ptr;
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
        MkvWriter* writer = Nan::ObjectWrap::Unwrap<MkvWriter>(info.Holder());
        writer->callback_ = info[1];
        // フレームを取得
        ttLibC_Frame *frame = writer->frameManager_->getFrame(info[0]->ToObject());
        if(frame == NULL) {
            puts("frameを復元することができてない。");
            info.GetReturnValue().Set(Nan::New(false));
            return;
        }
        if(!ttLibC_MkvWriter_write(
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
    ttLibC_MkvWriter *writer_;
    JsFrameManager *frameManager_;
    Local<Value> callback_;
};

NODE_MODULE(mkvWriter, MkvWriter::Init);
