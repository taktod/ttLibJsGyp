#include "pngDecoder.h"
#include "../frame.h"
#include "../util.h"

PngDecoder::PngDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_png;
#ifdef __ENABLE_LIBPNG__
  decoder_ = ttLibC_PngDecoder_make();
#endif
}

PngDecoder::~PngDecoder() {
#ifdef __ENABLE_LIBPNG__
  ttLibC_PngDecoder_close(&decoder_);
#endif
}

bool PngDecoder::decodeCallback(void *ptr, ttLibC_Bgr *bgr) {
  PngDecoder *decoder = (PngDecoder *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)bgr);
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

bool PngDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_LIBPNG__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_png) {
    puts("pngのみ処理可能です。");
    return false;
  }
  return ttLibC_PngDecoder_decode(
    decoder_,
    (ttLibC_Png *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}