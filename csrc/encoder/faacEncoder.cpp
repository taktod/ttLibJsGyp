﻿#include "faacEncoder.h"
#include "../frame.h"
#include "../util.h"

FaacEncoder::FaacEncoder(Local<Object> params) : Encoder() {
  type_ = get_faac;
#ifdef __ENABLE_FAAC_ENCODE__
  // type sampleRate channelNum bitrate(bit/sec)
  std::string type(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked())));
  uint32_t sampleRate = Uint32Value(Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked());
  uint32_t channelNum = Uint32Value(Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked());
  uint32_t bitrate = Uint32Value(Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked());
  ttLibC_FaacEncoder_Type faacEncoderType = FaacEncoderType_Low;
  if(type == "Main") {
    faacEncoderType = FaacEncoderType_Main;
  }
  else if(type == "SSR") {
    faacEncoderType = FaacEncoderType_SSR;
  }
  else if(type == "LTP") {
    faacEncoderType = FaacEncoderType_LTP;
  }
  encoder_ = ttLibC_FaacEncoder_make(
    faacEncoderType,
    sampleRate,
    channelNum,
    bitrate);
#endif
}

FaacEncoder::~FaacEncoder() {
#ifdef __ENABLE_FAAC_ENCODE__
  ttLibC_FaacEncoder_close(&encoder_);
#endif
}

bool FaacEncoder::encodeCallback(void *ptr, ttLibC_Aac *aac) {
  FaacEncoder *encoder = (FaacEncoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)aac);
  Local<Value> args[] = {
    jsFrame
  };
  Local<Value> result = callbackCall(callback, 1, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

bool FaacEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_FAAC_ENCODE__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_pcmS16) {
    puts("pcmS16のみ処理可能です。");
    return false;
  }
  return ttLibC_FaacEncoder_encode(
    encoder_,
    (ttLibC_PcmS16 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}
