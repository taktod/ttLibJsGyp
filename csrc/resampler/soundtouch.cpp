#include "soundtouch.h"
#include "../frame.h"

SoundtouchResampler::SoundtouchResampler(Local<Object> params) : Resampler() {
  type_ = grt_soundtouch;
#ifdef __ENABLE_SOUNDTOUCH__
  uint32_t sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  resampler_ = ttLibC_Soundtouch_make(sampleRate, channelNum);
#endif
}

SoundtouchResampler::~SoundtouchResampler() {
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch_close(&resampler_);
#endif
}

bool SoundtouchResampler::resampleCallback(void *ptr, ttLibC_Audio *audio) {
  SoundtouchResampler *resampler = (SoundtouchResampler *)ptr;
  Nan::Callback callback(resampler->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(resampler->jsFrame_);
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

bool SoundtouchResampler::resample(ttLibC_Frame *frame) {
#ifdef __ENABLE_SOUNDTOUCH__
  if(resampler_ == NULL) {
    puts("resamplerが準備されていません。");
    return false;
  }
  switch(frame->type) {
  case frameType_pcmS16:
  case frameType_pcmF32:
    break;
  default:
    puts("想定外なフレームでした。");
    return false;
  }
  return ttLibC_Soundtouch_resample(
    resampler_,
    (ttLibC_Audio *)frame,
    resampleCallback,
    this);
#else
  return false;
#endif
}

void SoundtouchResampler::setRate(double newRate) {
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch_setRate(resampler_, newRate);
#endif
}

void SoundtouchResampler::setTempo(double newTempo) {
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch_setTempo(resampler_, newTempo);
#endif
}

void SoundtouchResampler::setRateChange(double newRate) {
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch_setRateChange(resampler_, newRate);
#endif
}

void SoundtouchResampler::setTempoChange(double newTempo) {
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch_setTempoChange(resampler_, newTempo);
#endif
}

void SoundtouchResampler::setPitch(double newPitch) {
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch_setPitch(resampler_, newPitch);
#endif
}

void SoundtouchResampler::setPitchOctaves(double newPitch) {
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch_setPitchOctaves(resampler_, newPitch);
#endif
}

void SoundtouchResampler::setPitchSemiTones(double newPitch) {
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch_setPitchSemiTones(resampler_, newPitch);
#endif
}
