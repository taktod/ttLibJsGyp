#include "audioConverter.h"
#include "../frame.h"

AudioConverterEncoder::AudioConverterEncoder(Local<Object> params) : Encoder() {
  type_ = get_audioConverter;
#ifdef __ENABLE_APPLE__
  ttLibC_Frame_Type frameType = Frame::getFrameType(
    std::string(*String::Utf8Value(
      Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked()->ToString()))
  );
  uint32_t sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t bitrate = Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  encoder_ = ttLibC_AcEncoder_make(
    sampleRate,
    channelNum,
    bitrate,
    frameType);
#endif
}

AudioConverterEncoder::~AudioConverterEncoder() {
#ifdef __ENABLE_APPLE__
  ttLibC_AcEncoder_close(&encoder_);
#endif
}

bool AudioConverterEncoder::encodeCallback(void *ptr, ttLibC_Audio *audio) {
  AudioConverterEncoder *encoder = (AudioConverterEncoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)audio);
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

bool AudioConverterEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_APPLE__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame->type != frameType_pcmS16) {
    puts("pcmS16のみ処理可能です。");
    return false;
  }
  return ttLibC_AcEncoder_encode(
    encoder_,
    (ttLibC_PcmS16 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}
