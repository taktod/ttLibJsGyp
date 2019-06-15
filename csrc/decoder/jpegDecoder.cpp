#include "jpegDecoder.h"
#include "../frame.h"
#include "../util.h"

JpegDecoder::JpegDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_jpeg;
#ifdef __ENABLE_JPEG__
  decoder_ = ttLibC_JpegDecoder_make();
#endif
}

JpegDecoder::~JpegDecoder() {
#ifdef __ENABLE_JPEG__
  ttLibC_JpegDecoder_close(&decoder_);
#endif
}

bool JpegDecoder::decodeCallback(void *ptr, ttLibC_Yuv420 *yuv) {
  JpegDecoder *decoder = (JpegDecoder *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)yuv);
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

bool JpegDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_JPEG__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_jpeg) {
    puts("jpegのみ処理可能です。");
    return false;
  }
  return ttLibC_JpegDecoder_decode(
    decoder_,
    (ttLibC_Jpeg *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
