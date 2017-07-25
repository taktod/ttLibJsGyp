#include "speex.h"
#include "../frame.h"

SpeexDecoder::SpeexDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_speex;
#ifdef __ENABLE_SPEEX__
  uint32_t sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  decoder_ = ttLibC_SpeexDecoder_make(sampleRate, channelNum);
#endif
}

SpeexDecoder::~SpeexDecoder() {
#ifdef __ENABLE_SPEEX__
  ttLibC_SpeexDecoder_close(&decoder_);
#endif
}

bool SpeexDecoder::decodeCallback(void *ptr, ttLibC_PcmS16 *pcm) {
  SpeexDecoder *decoder = (SpeexDecoder *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)pcm);
  Local<Value> args[] = {
    jsFrame
  };
  Local<Value> result = callback.Call(1, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

bool SpeexDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_SPEEX__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_speex) {
    puts("speexのみ処理可能です。");
    return false;
  }
  return ttLibC_SpeexDecoder_decode(
    decoder_,
    (ttLibC_Speex *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
