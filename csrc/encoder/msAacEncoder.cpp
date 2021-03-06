﻿#include "msAacEncoder.h"
#include "../frame.h"
#include "../util.h"

MSAacEncoder::MSAacEncoder(Local<Object> params) : Encoder() {
  type_ = get_msAac;
#ifdef __ENABLE_WIN32__
  uint32_t sampleRate = Uint32Value(Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked());
  uint32_t channelNum = Uint32Value(Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked());
  uint32_t bitrate = Uint32Value(Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked());
  encoder_ = ttLibC_MsAacEncoder_make(
    sampleRate,
    channelNum,
    bitrate);
#endif
}

MSAacEncoder::~MSAacEncoder() {
#ifdef __ENABLE_WIN32__
  ttLibC_MsAacEncoder_close(&encoder_);
#endif
}

bool MSAacEncoder::encodeCallback(void *ptr, ttLibC_Aac *aac) {
  MSAacEncoder *encoder = (MSAacEncoder *)ptr;
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

bool MSAacEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_WIN32__
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
  return ttLibC_MsAacEncoder_encode(
    encoder_,
    (ttLibC_PcmS16 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}