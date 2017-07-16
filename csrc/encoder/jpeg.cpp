#include "jpeg.h"
#include "../frame.h"

JpegEncoder::JpegEncoder(Local<Object> params) : Encoder() {
  type_ = get_jpeg;
#ifdef __ENABLE_JPEG__
  uint32_t width            = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t height           = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t quality          = Nan::Get(params, Nan::New("quality").ToLocalChecked()).ToLocalChecked()->Uint32Value();
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

bool JpegEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_JPEG__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
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