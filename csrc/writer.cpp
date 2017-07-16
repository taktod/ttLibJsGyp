#include "predef.h"
#include "writer.h"
#include "frame.h"

#include <string>

void TTLIBJSGYP_CDECL Writer::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Writer").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "writeFrame", WriteFrame);
  SetPrototypeMethod(tpl, "writeInfo",  WriteInfo);
  SetPrototypeMethod(tpl, "setMode",    SetMode);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Writer").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Writer::New) {
  if(info.IsConstructCall()) {
    Writer *writer = new Writer(info);
    writer->Wrap(info.This());
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

NAN_METHOD(Writer::WriteFrame) {
  if(info.Length() == 2) {
    Writer *writer = Nan::ObjectWrap::Unwrap<Writer>(info.Holder());
    if(writer == NULL || writer->writer_ == NULL) {
      puts("writerがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    writer->callback_ = info[1];
    switch(writer->writer_->type) {
    case containerType_flv:
      if(ttLibC_FlvWriter_write(
          (ttLibC_FlvWriter *)writer->writer_,
          Frame::refFrame(info[0]),
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
        return;
      }
      break;
    case containerType_mkv:
    case containerType_webm:
      if(ttLibC_MkvWriter_write(
          writer->writer_,
          Frame::refFrame(info[0]),
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
        return;
      }
      break;
    case containerType_mp4:
      if(ttLibC_Mp4Writer_write(
          writer->writer_,
          Frame::refFrame(info[0]),
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
        return;
      }
      break;
    case containerType_mpegts:
      if(ttLibC_MpegtsWriter_write(
          writer->writer_,
          Frame::refFrame(info[0]),
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
        return;
      }
      break;
    default:
      puts("不明なコンテナでした");
      break;
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Writer::WriteInfo) {
  if(info.Length() == 1) {
    Writer *writer = Nan::ObjectWrap::Unwrap<Writer>(info.Holder());
    if(writer == NULL || writer->writer_ == NULL) {
      puts("writerがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    writer->callback_ = info[0];
    switch(writer->writer_->type) {
    case containerType_mpegts:
      if(ttLibC_MpegtsWriter_writeInfo(
          writer->writer_,
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
      }
      break;
    default:
      info.GetReturnValue().Set(true);
      return;
    }
  }
  info.GetReturnValue().Set(false);
}
NAN_METHOD(Writer::SetMode) {
  if(info.Length() == 1) {
    Writer *writer = Nan::ObjectWrap::Unwrap<Writer>(info.Holder());
    if(writer == NULL || writer->writer_ == NULL) {
      puts("writerがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    writer->writer_->mode = info[1]->Uint32Value();
  }
  info.GetReturnValue().Set(false);
}

Writer::Writer(Nan::NAN_METHOD_ARGS_TYPE info) {
  writer_ = NULL;
  // 基本このデータはtype length codec...となっている。
  // flvだけ例外でtype videoCodec audioCodecとなっている。
  std::string type(*String::Utf8Value(info[0]->ToString()));
  if(type == "flv") {
    writer_ = (ttLibC_ContainerWriter *)ttLibC_FlvWriter_make(
        Frame::getFrameType(std::string(*String::Utf8Value(info[1]->ToString()))),
        Frame::getFrameType(std::string(*String::Utf8Value(info[2]->ToString()))));
  }
  else {
    int unitDuration = info[1]->Uint32Value();
    Local<Array> codecs = Local<Array>::Cast(info[2]);
    int num = codecs->Length();
    ttLibC_Frame_Type *types = new ttLibC_Frame_Type[num];
    for(int i = 0;i < num;++ i) {
      types[i] = Frame::getFrameType(std::string(*String::Utf8Value(codecs->Get(i)->ToString())));
    }
    if(type == "mkv") {
      writer_ = ttLibC_MkvWriter_make_ex(types, num, unitDuration);
    }
    else if(type == "mp4") {
      writer_ = ttLibC_Mp4Writer_make_ex(types, num, unitDuration);
    }
    else if(type == "webm") {
      writer_ = ttLibC_MkvWriter_make_ex(types, num, unitDuration);
      writer_->type = containerType_webm;
    }
    else if(type == "mpegts") {
      writer_ = ttLibC_MpegtsWriter_make_ex(types, num, unitDuration);
    }
    else {
      writer_ = NULL;
    }
    delete[] types;
  }
}

Writer::~Writer() {
  ttLibC_ContainerWriter_close(&writer_);
}

bool Writer::writeCallback(
    void *ptr,
    void *data,
    size_t data_size) {
  // あとはこのデータをcallbackで応答すればよい。
  Writer       *writer   = (Writer *)ptr;
  Nan::Callback callback(writer->callback_.As<Function>());
  Local<Object> binary   = Nan::CopyBuffer((char *)data, data_size).ToLocalChecked();
  Local<Value>  args[]   = {
    Nan::Null(),
    binary
  };
  Local<Value> result = callback.Call(2, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}
