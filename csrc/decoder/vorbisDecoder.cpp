#include "vorbisDecoder.h"
#include "../frame.h"

VorbisDecoder::VorbisDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_vorbis;
#ifdef __ENABLE_VORBIS_DECODE__
  decoder_ = ttLibC_VorbisDecoder_make();
#endif
}

VorbisDecoder::~VorbisDecoder() {
#ifdef __ENABLE_VORBIS_DECODE__
  ttLibC_VorbisDecoder_close(&decoder_);
#endif
}

bool VorbisDecoder::decodeCallback(void *ptr, ttLibC_PcmF32 *pcm) {
  VorbisDecoder *decoder = (VorbisDecoder *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)pcm);
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

bool VorbisDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_VORBIS_DECODE__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_vorbis) {
    puts("vorbisのみ処理可能です。");
    return false;
  }
  return ttLibC_VorbisDecoder_decode(
    decoder_,
    (ttLibC_Vorbis *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
