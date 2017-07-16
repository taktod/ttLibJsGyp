#include "avcodec.h"
#include "../frame.h"

AvcodecDecoder::AvcodecDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_avcodec;
#ifdef __ENABLE_AVCODEC__
  ttLibC_Frame_Type frameType = Frame::getFrameType(
    std::string(*String::Utf8Value(
      Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked()->ToString()))
  );
  switch(frameType) {
  case frameType_h264:
    {
      Local<Value> width  = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked();
      Local<Value> height = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked();
      decoder_ = ttLibC_AvcodecVideoDecoder_make(frameType, width->Uint32Value(), height->Uint32Value());
    }
    break;
  case frameType_aac:
    {
      Local<Value> sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked();
      Local<Value> channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked();
      decoder_ = ttLibC_AvcodecAudioDecoder_make(frameType, sampleRate->Uint32Value(), channelNum->Uint32Value());
    }
    break;
  default:
    break;
  }
#endif
}

AvcodecDecoder::~AvcodecDecoder() {
#ifdef __ENABLE_AVCODEC__
  ttLibC_AvcodecDecoder_close(&decoder_);
#endif
}

bool AvcodecDecoder::decodeCallback(void *ptr, ttLibC_Frame *ttFrame) {
  AvcodecDecoder *decoder = (AvcodecDecoder *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, ttFrame);
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

bool AvcodecDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_AVCODEC__
  return ttLibC_AvcodecDecoder_decode(
    decoder_,
    frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}
