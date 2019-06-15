#include "speexEncoder.h"
#include "../frame.h"
#include "../util.h"

SpeexEncoder::SpeexEncoder(Local<Object> params) : Encoder() {
  type_ = get_opus;
#ifdef __ENABLE_SPEEX__
  uint32_t sampleRate = Uint32Value(Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked());
  uint32_t channelNum = Uint32Value(Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked());
  uint32_t quality    = Uint32Value(Nan::Get(params, Nan::New("quality").ToLocalChecked()).ToLocalChecked());
  encoder_ = ttLibC_SpeexEncoder_make(sampleRate, channelNum, quality);
#endif
}

SpeexEncoder::~SpeexEncoder() {
#ifdef __ENABLE_SPEEX__
  ttLibC_SpeexEncoder_close(&encoder_);
#endif
}

bool SpeexEncoder::encodeCallback(void *ptr, ttLibC_Speex *speex) {
  SpeexEncoder *encoder = (SpeexEncoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)speex);
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

bool SpeexEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_SPEEX__
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
  return ttLibC_SpeexEncoder_encode(
    encoder_,
    (ttLibC_PcmS16 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}
