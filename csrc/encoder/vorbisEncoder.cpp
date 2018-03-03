#include "vorbisEncoder.h"
#include "../frame.h"

VorbisEncoder::VorbisEncoder(Local<Object> params) : Encoder() {
  type_ = get_opus;
#ifdef __ENABLE_VORBIS_ENCODE__
  uint32_t sampleRate    = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum    = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  encoder_ = ttLibC_VorbisEncoder_make(sampleRate, channelNum);
#endif
}

VorbisEncoder::~VorbisEncoder() {
#ifdef __ENABLE_VORBIS_ENCODE__
  ttLibC_VorbisEncoder_close(&encoder_);
#endif
}

bool VorbisEncoder::encodeCallback(void *ptr, ttLibC_Vorbis *vorbis) {
  VorbisEncoder *encoder = (VorbisEncoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)vorbis);
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

bool VorbisEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_VORBIS_ENCODE__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame->type != frameType_pcmS16
  && frame->type != frameType_pcmF32) {
    puts("pcmのみ処理可能です。");
    return false;
  }
  return ttLibC_VorbisEncoder_encode(
    encoder_,
    (ttLibC_Audio *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}
