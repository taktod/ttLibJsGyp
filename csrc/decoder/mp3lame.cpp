#include "mp3lame.h"
#include "../frame.h"

Mp3lameDecoder::Mp3lameDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_mp3lame;
#ifdef __ENABLE_MP3LAME_DECODE__
  decoder_ = ttLibC_Mp3lameDecoder_make();
#endif
}

Mp3lameDecoder::~Mp3lameDecoder() {
#ifdef __ENABLE_MP3LAME_DECODE__
  ttLibC_Mp3lameDecoder_close(&decoder_);
#endif
}

bool Mp3lameDecoder::decodeCallback(void *ptr, ttLibC_PcmS16 *pcm) {
  Mp3lameDecoder *decoder = (Mp3lameDecoder *)ptr;
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

bool Mp3lameDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_MP3LAME_DECODE__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_mp3) {
    puts("mp3のみ処理可能です。");
    return false;
  }
  return ttLibC_Mp3lameDecoder_decode(
    decoder_,
    (ttLibC_Mp3 *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
