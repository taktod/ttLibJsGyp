#include "audio.h"
#include "../frame.h"

#include <ttLibC/resampler/audioResampler.h>

AudioResampler::AudioResampler(Local<Object> params) {
  type_ = grt_audio;
  frameType_ = Frame::getFrameType(
    std::string(*String::Utf8Value(
      Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked()->ToString()))
  );

  std::string subType(*String::Utf8Value(Nan::Get(params, Nan::New("subType").ToLocalChecked()).ToLocalChecked()->ToString()));
  switch(frameType_) {
  case frameType_pcmS16:
    if(subType == "bigEndian") {
      subType_ = PcmS16Type_bigEndian;
    }
    else if(subType == "bigEndianPlanar") {
      subType_ = PcmS16Type_bigEndian_planar;
    }
    else if(subType == "littleEndian") {
      subType_ = PcmS16Type_littleEndian;
    }
    else if(subType == "littleEndianPlanar") {
      subType_ = PcmS16Type_littleEndian_planar;
    }
    break;
  case frameType_pcmF32:
    if(subType == "planar") {
      subType_ = PcmF32Type_planar;
    }
    else if(subType == "interleave") {
      subType_ = PcmF32Type_interleave;
    }
    break;
  default:
    puts("不明");
    frameType_ = frameType_unknown;
    subType_ = -1;
    break;
  }
  channelNum_ = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  prevFrame_ = NULL;
}

AudioResampler::~AudioResampler() {
  ttLibC_Audio_close(&prevFrame_);
}

bool AudioResampler::resample(ttLibC_Frame *ttFrame) {
  if(ttFrame == NULL) {
    return true;
  }
  uint32_t subType = 0;
  uint32_t channelNum = 0;
  uint32_t targetChannelNum = channelNum_;
  ttLibC_Frame *resultFrame = NULL;
  switch(ttFrame->type) {
  case frameType_pcmS16:
    subType = ((ttLibC_PcmS16 *)ttFrame)->type;
    break;
  case frameType_pcmF32:
    subType = ((ttLibC_PcmF32 *)ttFrame)->type;
    break;
  default:
    puts("想定外なフレームタイプです。");
    return false;
  }
  channelNum = ((ttLibC_Audio *)ttFrame)->channel_num;
  if(targetChannelNum == 0) {
    targetChannelNum = channelNum;
  }
  // チャンネル数は設定されていない場合0の場合は、入力をそのまま使う。
  if(ttFrame->type == frameType_ && subType == subType_ && channelNum == targetChannelNum) {
    // 一致しているので、変換する必要がないです。
    resultFrame = ttFrame;
  }
  else {
    // 一致してないので、変換する必要があります。
    ttLibC_Audio *resampled = ttLibC_AudioResampler_convertFormat(
      prevFrame_,
      frameType_,
      subType_,
      targetChannelNum,
      (ttLibC_Audio *)ttFrame);
    if(resampled == NULL) {
      puts("resample失敗しました。");
      return false;
    }
    prevFrame_ = resampled;
    resultFrame = (ttLibC_Frame *)resampled;
  }
  // あとはこのresultFrameをcallbackで応答すればよい。
  Nan::Callback callback(callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(jsFrame_);
  Frame::setFrame(jsFrame, resultFrame);
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
