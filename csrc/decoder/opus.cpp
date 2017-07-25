#include "opus.h"
#include "../frame.h"

OpusDecoder::OpusDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_opus;
#ifdef __ENABLE_OPUS__
  uint32_t sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  decoder_ = ttLibC_OpusDecoder_make(sampleRate, channelNum);
#endif
}

OpusDecoder::~OpusDecoder() {
#ifdef __ENABLE_OPUS__
  ttLibC_OpusDecoder_close(&decoder_);
#endif
}

bool OpusDecoder::decodeCallback(void *ptr, ttLibC_PcmS16 *pcm) {
  OpusDecoder *decoder = (OpusDecoder *)ptr;
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

bool OpusDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_OPUS__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_opus) {
    puts("opusのみ処理可能です。");
    return false;
  }
  return ttLibC_OpusDecoder_decode(
    decoder_,
    (ttLibC_Opus *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
