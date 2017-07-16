#include "image.h"
#include "../frame.h"

#include <ttLibC/resampler/imageResampler.h>

ImageResampler::ImageResampler(Local<Object> params) {
  type_ = grt_image;
  frameType_ = Frame::getFrameType(
    std::string(*String::Utf8Value(
      Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked()->ToString()))
  );
  std::string subType(*String::Utf8Value(Nan::Get(params, Nan::New("subType").ToLocalChecked()).ToLocalChecked()->ToString()));
  switch(frameType_) {
  case frameType_bgr:
    if(subType == "bgr") {
      subType_ = BgrType_bgr;
    }
    else if(subType == "bgra") {
      subType_ = BgrType_bgra;
    }
    else if(subType == "abgr") {
      subType_ = BgrType_abgr;
    }
    break;
  case frameType_yuv420:
    if(subType == "planar") {
      subType_ = Yuv420Type_planar;
    }
    else if(subType == "semiPlanar") {
      subType_ = Yuv420Type_semiPlanar;
    }
    else if(subType == "yvuPlanar") {
      subType_ = Yvu420Type_planar;
    }
    else if(subType == "yvuSemiPlanar") {
      subType_ = Yvu420Type_semiPlanar;
    }
    break;
  default:
    puts("不明");
    frameType_ = frameType_unknown;
    subType_ = -1;
  }
  prevFrame_ = NULL;
}

ImageResampler::~ImageResampler() {
  ttLibC_Video_close(&prevFrame_);
}

bool ImageResampler::resample(ttLibC_Frame *ttFrame) {
  if(ttFrame == NULL) {
    return true;
  }
  if(ttFrame->type == frameType_yuv420 && frameType_ == frameType_bgr) {
    ttLibC_Bgr *b = ttLibC_ImageResampler_makeBgrFromYuv420((ttLibC_Bgr *)prevFrame_, (ttLibC_Bgr_Type)subType_, (ttLibC_Yuv420 *)ttFrame);
    if(b == NULL) {
      puts("resample失敗しました。");
      return false;
    }
    prevFrame_ = (ttLibC_Video *)b;
  }
  else if(ttFrame->type == frameType_bgr && frameType_ == frameType_yuv420) {
    ttLibC_Yuv420 *y = ttLibC_ImageResampler_makeYuv420FromBgr((ttLibC_Yuv420 *)prevFrame_, (ttLibC_Yuv420_Type)subType_, (ttLibC_Bgr *)ttFrame);
    if(y == NULL) {
      puts("resample失敗しました。");
      return false;
    }
    prevFrame_ = (ttLibC_Video *)y;
  }
  else {
    puts("想定外の変換でした。処理を中断します。");
    return false;
  }
  Nan::Callback callback(callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)prevFrame_);
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
