#include "openh264Decoder.h"
#include "../frame.h"

Openh264Decoder::Openh264Decoder(Local<Object> params) : Decoder() {
  type_ = gdt_openh264;
#ifdef __ENABLE_OPENH264__
  decoder_ = ttLibC_Openh264Decoder_make();
#endif
}

Openh264Decoder::~Openh264Decoder() {
#ifdef __ENABLE_OPENH264__
  ttLibC_Openh264Decoder_close(&decoder_);
#endif
}

bool Openh264Decoder::decodeCallback(void *ptr, ttLibC_Yuv420 *yuv) {
  Openh264Decoder *decoder = (Openh264Decoder *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)yuv);
  Local<Value> args[] = {
    jsFrame
  };
  Local<Value> result = Nan::Call(callback, Nan::New<v8::Object>(), 1, args).ToLocalChecked();
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

bool Openh264Decoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_OPENH264__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_h264) {
    puts("h264のみ処理可能です。");
    return false;
  }
  return ttLibC_Openh264Decoder_decode(
    decoder_,
    (ttLibC_H264 *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
