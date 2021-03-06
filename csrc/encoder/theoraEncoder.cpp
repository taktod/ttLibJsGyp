﻿#include "theoraEncoder.h"
#include "../frame.h"
#include "../util.h"

TheoraEncoder::TheoraEncoder(Local<Object> params) : Encoder() {
  type_ = get_theora;
#ifdef __ENABLE_THEORA__
  uint32_t width            = Uint32Value(Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked());
  uint32_t height           = Uint32Value(Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked());
  uint32_t quality          = Uint32Value(Nan::Get(params, Nan::New("quality").ToLocalChecked()).ToLocalChecked());
  uint32_t bitrate          = Uint32Value(Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked());
  uint32_t keyFrameInterval = Uint32Value(Nan::Get(params, Nan::New("keyFrameInterval").ToLocalChecked()).ToLocalChecked());
  encoder_ = ttLibC_TheoraEncoder_make_ex(width, height, quality, bitrate, keyFrameInterval);
#endif
}

TheoraEncoder::~TheoraEncoder() {
#ifdef __ENABLE_THEORA__
  ttLibC_TheoraEncoder_close(&encoder_);
#endif
}

bool TheoraEncoder::encodeCallback(void *ptr, ttLibC_Theora *theora) {
  TheoraEncoder *encoder = (TheoraEncoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)theora);
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

bool TheoraEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_THEORA__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_yuv420) {
    puts("yuv420のみ処理可能です。");
    return false;
  }
  return ttLibC_TheoraEncoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}