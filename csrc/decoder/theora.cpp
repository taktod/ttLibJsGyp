#include "theora.h"
#include "../frame.h"

TheoraDecoder::TheoraDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_theora;
#ifdef __ENABLE_THEORA__
  decoder_ = ttLibC_TheoraDecoder_make();
#endif
}

TheoraDecoder::~TheoraDecoder() {
#ifdef __ENABLE_THEORA__
  ttLibC_TheoraDecoder_close(&decoder_);
#endif
}

bool TheoraDecoder::decodeCallback(void *ptr, ttLibC_Yuv420 *yuv) {
  TheoraDecoder *decoder = (TheoraDecoder *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)yuv);
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

bool TheoraDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_THEORA__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_theora) {
    puts("theoraのみ処理可能です。");
    return false;
  }
  return ttLibC_TheoraDecoder_decode(
    decoder_,
    (ttLibC_Theora *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
