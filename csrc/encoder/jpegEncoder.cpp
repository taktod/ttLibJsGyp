#include "jpegEncoder.h"
#include "../frame.h"
#include "../util.h"

JpegEncoder::JpegEncoder(Local<Object> params) : Encoder() {
  type_ = get_jpeg;
#ifdef __ENABLE_JPEG__
  uint32_t width   = Uint32Value(Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked());
  uint32_t height  = Uint32Value(Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked());
  uint32_t quality = Uint32Value(Nan::Get(params, Nan::New("quality").ToLocalChecked()).ToLocalChecked());
  encoder_ = ttLibC_JpegEncoder_make(width, height, quality);
#endif
}

JpegEncoder::~JpegEncoder() {
#ifdef __ENABLE_JPEG__
  ttLibC_JpegEncoder_close(&encoder_);
#endif
}

bool JpegEncoder::encodeCallback(void *ptr, ttLibC_Jpeg *jpeg) {
  JpegEncoder *encoder = (JpegEncoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)jpeg);
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

bool JpegEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_JPEG__
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
  return ttLibC_JpegEncoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}

bool JpegEncoder::setQuality(uint32_t quality) {
#ifdef __ENABLE_JPEG__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  return ttLibC_JpegEncoder_setQuality(encoder_, quality);
#else
  return false;
#endif
}
