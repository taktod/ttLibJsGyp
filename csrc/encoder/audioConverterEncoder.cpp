#include "audioConverterEncoder.h"
#include "../frame.h"
#include "../util.h"

AudioConverterEncoder::AudioConverterEncoder(Local<Object> params) : Encoder() {
  type_ = get_audioConverter;
#ifdef __ENABLE_APPLE__
  ttLibC_Frame_Type frameType = Frame::getFrameType(
    std::string(*String::Utf8Value(v8::Isolate::GetCurrent(),
      ToString(Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked())))
  );
  uint32_t sampleRate = Uint32Value(Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked());
  uint32_t channelNum = Uint32Value(Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked());
  uint32_t bitrate = Uint32Value(Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked());
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

bool AudioConverterEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_APPLE__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame == NULL) {
    return true;
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
