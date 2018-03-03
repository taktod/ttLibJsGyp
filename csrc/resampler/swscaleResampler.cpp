#include "swscaleResampler.h"
#include "../frame.h"

SwscaleResampler::SwscaleResampler(Local<Object> params) : Resampler() {
  type_ = grt_swscale;
#ifdef __ENABLE_SWSCALE__
  std::string inTypeStr(*String::Utf8Value(Nan::Get(params, Nan::New("inType").ToLocalChecked()).ToLocalChecked()->ToString()));
  std::string inSubTypeStr(*String::Utf8Value(Nan::Get(params, Nan::New("inSubType").ToLocalChecked()).ToLocalChecked()->ToString()));
  ttLibC_Frame_Type inType = Frame::getFrameType(inTypeStr);
  uint32_t inSubType = getSubType(inType, inSubTypeStr);
  uint32_t inWidth = Nan::Get(params, Nan::New("inWidth").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t inHeight = Nan::Get(params, Nan::New("inHeight").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  std::string outTypeStr(*String::Utf8Value(Nan::Get(params, Nan::New("outType").ToLocalChecked()).ToLocalChecked()->ToString()));
  std::string outSubTypeStr(*String::Utf8Value(Nan::Get(params, Nan::New("outSubType").ToLocalChecked()).ToLocalChecked()->ToString()));
  ttLibC_Frame_Type outType = Frame::getFrameType(outTypeStr);
  uint32_t outSubType = getSubType(outType, outSubTypeStr);
  uint32_t outWidth = Nan::Get(params, Nan::New("outWidth").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t outHeight = Nan::Get(params, Nan::New("outHeight").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  std::string mode(*String::Utf8Value(Nan::Get(params, Nan::New("mode").ToLocalChecked()).ToLocalChecked()->ToString()));
	ttLibC_SwscaleResampler_Mode scaleMode = SwscaleResampler_FastBiLinear;
	if(mode == "X") {
		scaleMode = SwscaleResampler_X;
	}
	else if(mode == "Area") {
		scaleMode = SwscaleResampler_Area;
	}
	else if(mode == "Sinc") {
		scaleMode = SwscaleResampler_Sinc;
	}
	else if(mode == "Point") {
		scaleMode = SwscaleResampler_Point;
	}
	else if(mode == "Gauss") {
		scaleMode = SwscaleResampler_Gauss;
	}
	else if(mode == "Spline") {
		scaleMode = SwscaleResampler_Spline;
	}
	else if(mode == "Bicubic") {
		scaleMode = SwscaleResampler_Bicubic;
	}
	else if(mode == "Lanczos") {
		scaleMode = SwscaleResampler_Lanczos;
	}
	else if(mode == "Bilinear") {
		scaleMode = SwscaleResampler_Bilinear;
	}
	else if(mode == "Bicublin") {
		scaleMode = SwscaleResampler_Bicublin;
	}
	else if(mode == "FastBilinear") {
		scaleMode = SwscaleResampler_FastBiLinear;
	}
  resampler_ = ttLibC_SwscaleResampler_make(
    inType, inSubType, inWidth, inHeight,
    outType, outSubType, outWidth, outHeight,
    scaleMode
  );
#endif
}

uint32_t SwscaleResampler::getSubType(
    ttLibC_Frame_Type type,
    std::string subType) {
	switch(type) {
	case frameType_yuv420:
		{
			if(subType == "planar") {
				return Yuv420Type_planar;
			}
			else if(subType == "yvuPlanar") {
				return Yvu420Type_planar;
			}
			else if(subType == "semiPlanar") {
				return Yuv420Type_semiPlanar;
			}
			else if(subType == "yvuSemiPlanar") {
				return Yvu420Type_semiPlanar;
			}
		}
		break;
	case frameType_bgr:
		{
			if(subType == "bgr") {
				return BgrType_bgr;
			}
			else if(subType == "bgra") {
				return BgrType_bgra;
			}
			else if(subType == "abgr") {
				return BgrType_abgr;
			}
		}
		break;
	default:
		break;
	}
	return 99;
}


SwscaleResampler::~SwscaleResampler() {
#ifdef __ENABLE_SWSCALE__
  ttLibC_SwscaleResampler_close(&resampler_);
#endif
}

bool SwscaleResampler::resampleCallback(void *ptr, ttLibC_Frame *video) {
  SwscaleResampler *resampler = (SwscaleResampler *)ptr;
  Nan::Callback callback(resampler->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(resampler->jsFrame_);
  Frame::setFrame(jsFrame, video);
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

bool SwscaleResampler::resample(ttLibC_Frame *frame) {
#ifdef __ENABLE_SWSCALE__
  if(resampler_ == NULL) {
    puts("resamplerが準備されていません。");
    return false;
  }
  switch(frame->type) {
  case frameType_bgr:
  case frameType_yuv420:
    break;
  default:
    puts("想定外なフレームでした。");
    return false;
  }
  return ttLibC_SwscaleResampler_resample(
    resampler_,
    frame,
    resampleCallback,
    this);
#else
  return false;
#endif
}
