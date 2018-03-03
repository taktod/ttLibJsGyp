#include "libyuvRotateResampler.h"
#include "../frame.h"

LibyuvRotateResampler::LibyuvRotateResampler(Local<Object> params) {
  type_ = grt_libyuvrotate;
  prevFrame_ = nullptr;
#ifdef __ENABLE_LIBYUV__
  uint32_t degree = Nan::Get(params, Nan::New("degree").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  switch(degree) {
  default:
    puts("回転角度は0 90 180 270のみサポートしています。");
  case 0:
    mode_ = LibyuvRotate_0;
    break;
  case 90:
    mode_ = LibyuvRotate_90;
    break;
  case 180:
    mode_ = LibyuvRotate_180;
    break;
  case 270:
    mode_ = LibyuvRotate_270;
    break;
  }
#endif
}

LibyuvRotateResampler::~LibyuvRotateResampler() {
  ttLibC_Yuv420_close(&prevFrame_);
}

bool LibyuvRotateResampler::resample(ttLibC_Frame *ttFrame) {
#ifdef __ENABLE_LIBYUV__
  switch(ttFrame->type) {
  case frameType_yuv420:
    break;
  default:
    puts("想定外なフレームでした。");
    return false;
  }
  // あとはリサンプルすれば良いわけだが・・・
  ttLibC_Yuv420 *y = ttLibC_LibyuvResampler_rotate(
    prevFrame_,
    (ttLibC_Yuv420 *)ttFrame,
    mode_);
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
  Local<Value> result = Nan::Call(callback, Nan::New<v8::Object>(), 1, args).ToLocalChecked();
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
#endif
  return false;
}
