#include "opus.h"
#include "../frame.h"

OpusEncoder::OpusEncoder(Local<Object> params) : Encoder() {
  type_ = get_opus;
#ifdef __ENABLE_OPUS__
  uint32_t sampleRate    = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum    = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t unitSampleNum = Nan::Get(params, Nan::New("unitSampleNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  encoder_ = ttLibC_OpusEncoder_make(sampleRate, channelNum, unitSampleNum);
#endif
}

OpusEncoder::~OpusEncoder() {
#ifdef __ENABLE_OPUS__
  ttLibC_OpusEncoder_close(&encoder_);
#endif
}

bool OpusEncoder::encodeCallback(void *ptr, ttLibC_Opus *opus) {
  OpusEncoder *encoder = (OpusEncoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)opus);
  Local<Value> args[] = {
    Nan::Null(),
    jsFrame
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

bool OpusEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_OPUS__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame->type != frameType_pcmS16) {
    puts("pcmS16のみ処理可能です。");
    return false;
  }
  return ttLibC_OpusEncoder_encode(
    encoder_,
    (ttLibC_PcmS16 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}
