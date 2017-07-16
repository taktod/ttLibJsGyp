#include "predef.h"
#include "reader.h"
#include "frame.h"

#include <string>

void TTLIBJSGYP_CDECL Reader::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Reader").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "readFrame", ReadFrame);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Reader").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Reader::New) {
  if(info.IsConstructCall()) {
    // パラメーターがおかしかったら例外を投げたいところ
    Reader *reader = new Reader(info);
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

NAN_METHOD(Reader::ReadFrame) {
  if(info.Length() == 2) {
    Local<Value> binary   = info[0];
    Local<Value> callback = info[1];
    if(binary->IsUint8Array()
    || binary->IsArrayBuffer()) {
      void   *data      = (void *)node::Buffer::Data(binary->ToObject());
      size_t  data_size = node::Buffer::Length(binary->ToObject());
      Reader *reader    = Nan::ObjectWrap::Unwrap<Reader>(info.Holder());
      if(reader == NULL || reader->reader_ == NULL) {
        puts("readerがありません。");
        info.GetReturnValue().Set(false);
        return;
      }
      reader->callback_ = callback;
      switch(reader->reader_->type) {
      case containerType_flv:
        if(ttLibC_FlvReader_read(
            (ttLibC_FlvReader *)reader->reader_,
            data,
            data_size,
            flvCallback,
            reader)) {
          info.GetReturnValue().Set(true);
          return;
        }
        break;
      case containerType_mkv:
        if(ttLibC_MkvReader_read(
            (ttLibC_MkvReader *)reader->reader_,
            data,
            data_size,
            mkvCallback,
            reader)) {
          info.GetReturnValue().Set(true);
          return;
        }
        break;
      case containerType_mp4:
        if(ttLibC_Mp4Reader_read(
            (ttLibC_Mp4Reader *)reader->reader_,
            data,
            data_size,
            mp4Callback,
            reader)) {
          info.GetReturnValue().Set(true);
          return;
        }
        break;
      case containerType_mpegts:
        if(ttLibC_MpegtsReader_read(
            (ttLibC_MpegtsReader *)reader->reader_,
            data,
            data_size,
            mpegtsCallback,
            reader)) {
          info.GetReturnValue().Set(true);
          return;
        }
        break;
      default:
        puts("不明なコンテナでした。");
        break;
      }
    }
  }
  info.GetReturnValue().Set(false);
}

Reader::Reader(Nan::NAN_METHOD_ARGS_TYPE info) {
  std::string type(*String::Utf8Value(info[0]->ToString()));
  if(type == "flv") {
    reader_ = (ttLibC_ContainerReader *)ttLibC_FlvReader_make();
  }
  else if(type == "mkv" || type == "webm") {
    reader_ = (ttLibC_ContainerReader *)ttLibC_MkvReader_make();
  }
  else if(type == "mp4") {
    reader_ = (ttLibC_ContainerReader *)ttLibC_Mp4Reader_make();
  }
  else if(type == "mpegts") {
    reader_ = (ttLibC_ContainerReader *)ttLibC_MpegtsReader_make();
  }
  else {
    reader_ = NULL;
  }
  jsVideoFrame_.Reset(Frame::newInstance());
  jsAudioFrame_.Reset(Frame::newInstance());
}

Reader::~Reader() {
  ttLibC_ContainerReader_close(&reader_);
  jsVideoFrame_.Reset();
  jsAudioFrame_.Reset();
}

bool Reader::frameCallback(
    void *ptr,
    ttLibC_Frame *ttFrame) {
  Reader       *reader   = (Reader *)ptr;
  Nan::Callback callback(reader->callback_.As<Function>());
  Local<Object> jsFrame;
  if(ttLibC_Frame_isAudio(ttFrame)) {
    jsFrame = Nan::New(reader->jsAudioFrame_);
  }
  else if(ttLibC_Frame_isVideo(ttFrame)) {
    jsFrame = Nan::New(reader->jsVideoFrame_);
  }
  Frame::setFrame(jsFrame, ttFrame);
  Local<Value> args[] = {
    Nan::Null(),
    jsFrame // あとはここにframe情報をうまく折り込めばOKの予定だが・・・
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

bool Reader::flvCallback(
    void *ptr,
    ttLibC_Flv *flv) {
  return ttLibC_Flv_getFrame(
      flv,
      frameCallback,
      ptr);
}

bool Reader::mkvCallback(
    void *ptr,
    ttLibC_Mkv *mkv) {
  return ttLibC_Mkv_getFrame(
      mkv,
      frameCallback,
      ptr);
}

bool Reader::mp4Callback(
    void *ptr,
    ttLibC_Mp4 *mp4) {
  return ttLibC_Mp4_getFrame(
      mp4,
      frameCallback,
      ptr);
}

bool Reader::mpegtsCallback(
    void *ptr,
    ttLibC_Mpegts *mpegts) {
  return ttLibC_Mpegts_getFrame(
      mpegts,
      frameCallback,
      ptr);
}
