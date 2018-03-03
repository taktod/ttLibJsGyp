#include "audioConverterDecoder.h"
#include "../frame.h"

AudioConverterDecoder::AudioConverterDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_audioConverter;
#ifdef __ENABLE_APPLE__
  ttLibC_Frame_Type frameType = Frame::getFrameType(
    std::string(*String::Utf8Value(
      Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked()->ToString()))
  );
  uint32_t sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  decoder_ = ttLibC_AcDecoder_make(sampleRate, channelNum, frameType);
#endif
}

AudioConverterDecoder::~AudioConverterDecoder() {
#ifdef __ENABLE_APPLE__
  ttLibC_AcDecoder_close(&decoder_);
#endif
}

bool AudioConverterDecoder::decodeCallback(void *ptr, ttLibC_PcmS16 *pcm) {
  AudioConverterDecoder *decoder = (AudioConverterDecoder *)ptr;
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

bool AudioConverterDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_APPLE__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type == decoder_->frame_type) {
    puts("指定したフレームのみ処理可能です。");
    return false;
  }
  return ttLibC_AcDecoder_decode(
    decoder_,
    (ttLibC_Audio *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
