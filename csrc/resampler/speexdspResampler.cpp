﻿#include "speexdspResampler.h"
#include "../frame.h"
#include "../util.h"

#include <string>

SpeexdspResampler::SpeexdspResampler(Local<Object> params) {
  type_ = grt_speexdsp;
#ifdef __ENABLE_SPEEXDSP__
  uint32_t channelNum    = Uint32Value(Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked());
  uint32_t inSampleRate  = Uint32Value(Nan::Get(params, Nan::New("inSampleRate").ToLocalChecked()).ToLocalChecked());
  uint32_t outSampleRate = Uint32Value(Nan::Get(params, Nan::New("outSampleRate").ToLocalChecked()).ToLocalChecked());
  uint32_t quality       = Uint32Value(Nan::Get(params, Nan::New("quality").ToLocalChecked()).ToLocalChecked());
  pcm_ = NULL;
  resampler_ = ttLibC_SpeexdspResampler_make(channelNum, inSampleRate, outSampleRate, quality);
#endif
}

SpeexdspResampler::~SpeexdspResampler() {
#ifdef __ENABLE_SPEEXDSP__
  ttLibC_SpeexdspResampler_close(&resampler_);
#endif
}

bool SpeexdspResampler::resample(ttLibC_Frame *frame) {
#ifdef __ENABLE_SPEEXDSP__
  if(resampler_ == NULL) {
    puts("resamplerが準備されていません。");
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_pcmS16) {
    puts("pcmS16のみ処理可能です。");
    return false;
  }
  ttLibC_PcmS16 *pcm = (ttLibC_PcmS16 *)frame;
  if(pcm->inherit_super.channel_num != resampler_->channel_num) {
    puts("チャンネル数が一致しません。");
    return false;
  }
  if(pcm->inherit_super.sample_rate != resampler_->input_sample_rate) {
    puts("入力サンプルレートが一致しません。");
    return false;
  }
  ttLibC_PcmS16 *p = ttLibC_SpeexdspResampler_resample(resampler_, pcm_, pcm);
  if(p == NULL) {
    puts("リサンプル失敗しました。");
    return false;
  }
  pcm_ = p;
  Nan::Callback callback(callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)pcm_);
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
#endif
  return false;
}