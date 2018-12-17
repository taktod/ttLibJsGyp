#include "libyuvScaleResampler.h"
#include "../frame.h"

LibyuvScaleResampler::LibyuvScaleResampler(Local<Object> params) {
  type_ = grt_libyuvscale;
  prevFrame_ = nullptr;
#ifdef __ENABLE_LIBYUV__
  width_  = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  height_ = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  std::string yModeStr(*String::Utf8Value(Nan::Get(params, Nan::New("yMode").ToLocalChecked()).ToLocalChecked()->ToString()));
  std::string uModeStr(*String::Utf8Value(Nan::Get(params, Nan::New("uMode").ToLocalChecked()).ToLocalChecked()->ToString()));
  std::string vModeStr(*String::Utf8Value(Nan::Get(params, Nan::New("vMode").ToLocalChecked()).ToLocalChecked()->ToString()));
  if(yModeStr == "Linear") {
    yMode_ = LibyuvFilter_Linear;
  }
  else if(yModeStr == "Bilinear") {
    yMode_ = LibyuvFilter_Bilinear;
  }
  else if(yModeStr == "Box") {
    yMode_ = LibyuvFilter_Box;
  }
  else {
    yMode_ = LibyuvFilter_None;
  }
  if(uModeStr == "Linear") {
    uMode_ = LibyuvFilter_Linear;
  }
  else if(uModeStr == "Bilinear") {
    uMode_ = LibyuvFilter_Bilinear;
  }
  else if(uModeStr == "Box") {
    uMode_ = LibyuvFilter_Box;
  }
  else {
    uMode_ = LibyuvFilter_None;
  }
  if(vModeStr == "Linear") {
    vMode_ = LibyuvFilter_Linear;
  }
  else if(vModeStr == "Bilinear") {
    vMode_ = LibyuvFilter_Bilinear;
  }
  else if(vModeStr == "Box") {
    vMode_ = LibyuvFilter_Box;
  }
  else {
    vMode_ = LibyuvFilter_None;
  }
#endif
}

LibyuvScaleResampler::~LibyuvScaleResampler() {
  ttLibC_Yuv420_close(&prevFrame_);
}

bool LibyuvScaleResampler::resample(ttLibC_Frame *ttFrame) {
#ifdef __ENABLE_LIBYUV__
  if(ttFrame == NULL) {
    return true;
  }
  switch(ttFrame->type) {
  case frameType_yuv420:
    break;
  default:
    puts("想定外なフレームでした。");
    return false;
  }
  // あとはリサンプルすれば良いわけだが・・・
  ttLibC_Yuv420 *y = ttLibC_LibyuvResampler_resize(
    prevFrame_,
    width_,
    height_,
    (ttLibC_Yuv420 *)ttFrame,
    yMode_,
    uMode_,
    vMode_);
  if(y == nullptr) {
    puts("resample失敗しました。");
    return false;
  }
  prevFrame_ = y;
  Nan::Callback callback(callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)prevFrame_);
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
#endif
  return false;
}