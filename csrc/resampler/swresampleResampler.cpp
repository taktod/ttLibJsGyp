#include "swresampleResampler.h"
#include "../frame.h"

SwresampleResampler::SwresampleResampler(Local<Object> params) : Resampler() {
  type_ = grt_swresample;
#ifdef __ENABLE_SWRESAMPLE__
  std::string inTypeStr(*String::Utf8Value(Nan::Get(params, Nan::New("inType").ToLocalChecked()).ToLocalChecked()->ToString()));
  std::string inSubTypeStr(*String::Utf8Value(Nan::Get(params, Nan::New("inSubType").ToLocalChecked()).ToLocalChecked()->ToString()));
  ttLibC_Frame_Type inType = Frame::getFrameType(inTypeStr);
  uint32_t inSubType = getSubType(inType, inSubTypeStr);
  uint32_t inSampleRate = Nan::Get(params, Nan::New("inSampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t inChannelNum = Nan::Get(params, Nan::New("inChannelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  std::string outTypeStr(*String::Utf8Value(Nan::Get(params, Nan::New("outType").ToLocalChecked()).ToLocalChecked()->ToString()));
  std::string outSubTypeStr(*String::Utf8Value(Nan::Get(params, Nan::New("outSubType").ToLocalChecked()).ToLocalChecked()->ToString()));
  ttLibC_Frame_Type outType = Frame::getFrameType(outTypeStr);
  uint32_t outSubType = getSubType(outType, outSubTypeStr);
  uint32_t outSampleRate = Nan::Get(params, Nan::New("outSampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t outChannelNum = Nan::Get(params, Nan::New("outChannelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  resampler_ = ttLibC_SwresampleResampler_make(
    inType, inSubType, inSampleRate, inChannelNum,
    outType, outSubType, outSampleRate, outChannelNum);
#endif
}

uint32_t SwresampleResampler::getSubType(
    ttLibC_Frame_Type type,
    std::string subType) {
	switch(type) {
	case frameType_pcmS16:
		{
			if(subType == "bigEndian") {
				return PcmS16Type_bigEndian;
			}
			else if(subType == "littleEndian") {
				return PcmS16Type_littleEndian;
			}
			else if(subType == "bigEndianPlanar") {
				return PcmS16Type_bigEndian_planar;
			}
			else if(subType == "littleEndianPlanar") {
				return PcmS16Type_littleEndian_planar;
			}
			else {
				puts("unexpected pcmS16 subType");
			}
		}
		break;
	case frameType_pcmF32:
		{
			if(subType == "planar") {
				return PcmF32Type_planar;
			}
			else if(subType == "interleave") {
				return PcmF32Type_interleave;
			}
			else {
				puts("unexpected pcmF32 subType");
			}
		}
		break;
	default:
		puts("unexpected frametype.");
	}
	return 99;
}


SwresampleResampler::~SwresampleResampler() {
#ifdef __ENABLE_SWRESAMPLE__
  ttLibC_SwresampleResampler_close(&resampler_);
#endif
}

bool SwresampleResampler::resampleCallback(void *ptr, ttLibC_Frame *audio) {
  SwresampleResampler *resampler = (SwresampleResampler *)ptr;
  Nan::Callback callback(resampler->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(resampler->jsFrame_);
  Frame::setFrame(jsFrame, audio);
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

bool SwresampleResampler::resample(ttLibC_Frame *frame) {
#ifdef __ENABLE_SWRESAMPLE__
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
  return ttLibC_SwresampleResampler_resample(
    resampler_,
    frame,
    resampleCallback,
    this);
#else
  return false;
#endif
}