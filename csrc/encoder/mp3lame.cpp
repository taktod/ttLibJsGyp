#include "mp3lame.h"
#include "../frame.h"

Mp3lameEncoder::Mp3lameEncoder(Local<Object> params) : Encoder() {
  type_ = get_mp3lame;
#ifdef __ENABLE_MP3LAME_ENCODE__
  // sampleRate channelNum, quality
  uint32_t sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t quality = Nan::Get(params, Nan::New("quality").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  encoder_ = ttLibC_Mp3lameEncoder_make(
    sampleRate,
    channelNum,
    quality);
#endif
}

Mp3lameEncoder::~Mp3lameEncoder() {
#ifdef __ENABLE_MP3LAME_ENCODE__
  ttLibC_Mp3lameEncoder_close(&encoder_);
#endif
}

bool Mp3lameEncoder::encodeCallback(void *ptr, ttLibC_Mp3 *mp3) {
  Mp3lameEncoder *encoder = (Mp3lameEncoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)mp3);
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

bool Mp3lameEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_MP3LAME_ENCODE__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame->type != frameType_pcmS16) {
    puts("pcmS16のみ処理可能です。");
    return false;
  }
  return ttLibC_Mp3lameEncoder_encode(
    encoder_,
    (ttLibC_PcmS16 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}