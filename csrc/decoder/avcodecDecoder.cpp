﻿#include "avcodecDecoder.h"
#include "../frame.h"
#include "../util.h"

AvcodecDecoder::AvcodecDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_avcodec;
#ifdef __ENABLE_AVCODEC__
  ttLibC_Frame_Type frameType = Frame::getFrameType(
    std::string(*String::Utf8Value(v8::Isolate::GetCurrent(),
      ToString(Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked())))
  );
  if(ttLibC_isVideo(frameType)) {
    Local<Value> width  = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked();
    Local<Value> height = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked();
    decoder_ = ttLibC_AvcodecVideoDecoder_make(frameType, Uint32Value(width), Uint32Value(height));
  }
  else if(ttLibC_isAudio(frameType)) {
    Local<Value> sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked();
    Local<Value> channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked();
    decoder_ = ttLibC_AvcodecAudioDecoder_make(frameType, Uint32Value(sampleRate), Uint32Value(channelNum));
  }
#endif
}

AvcodecDecoder::~AvcodecDecoder() {
#ifdef __ENABLE_AVCODEC__
  ttLibC_AvcodecDecoder_close(&decoder_);
#endif
}

bool AvcodecDecoder::decodeCallback(void *ptr, ttLibC_Frame *ttFrame) {
  AvcodecDecoder *decoder = (AvcodecDecoder *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, ttFrame);
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

bool AvcodecDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_AVCODEC__
  return ttLibC_AvcodecDecoder_decode(
    decoder_,
    frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
