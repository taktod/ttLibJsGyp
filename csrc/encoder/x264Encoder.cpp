#include "x264Encoder.h"
#include "../frame.h"
#include "../util.h"
#ifdef __ENABLE_X264__
# include <x264.h>
#endif

X264Encoder::X264Encoder(Local<Object> params) : Encoder() {
  type_ = get_x264;
#ifdef __ENABLE_X264__
  // width height preset tunr profile paramを送る
  uint32_t width  = Uint32Value(Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked());
  uint32_t height = Uint32Value(Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked());
  String::Utf8Value preset( v8::Isolate::GetCurrent(), ToString(Nan::Get(params, Nan::New("preset").ToLocalChecked()).ToLocalChecked()));
  String::Utf8Value tune(   v8::Isolate::GetCurrent(), ToString(Nan::Get(params, Nan::New("tune").ToLocalChecked()).ToLocalChecked()));
  String::Utf8Value profile(v8::Isolate::GetCurrent(), ToString(Nan::Get(params, Nan::New("profile").ToLocalChecked()).ToLocalChecked()));
  Local<Object> param = ToObject(Nan::Get(params, Nan::New("param").ToLocalChecked()).ToLocalChecked());
  x264_param_t x264Param;
  ttLibC_X264Encoder_getDefaultX264ParamTWithPresetTune(
    &x264Param,
    width,
    height,
    *preset,
    *tune);
  {
    Local<Array> keys = param->GetPropertyNames(Nan::GetCurrentContext()).ToLocalChecked();
    for(int i = 0, max = keys->Length();i < max;++ i) {
      String::Utf8Value key(v8::Isolate::GetCurrent(), ToString(Nan::Get(keys, i).ToLocalChecked()));
      String::Utf8Value value(v8::Isolate::GetCurrent(), ToString(Nan::Get(param, Nan::New(*key).ToLocalChecked()).ToLocalChecked()));
      int result = x264_param_parse(&x264Param, *key, *value);
      if(result < 0) {
        printf("%s %s:パラメーター設定失敗しました。\n", *key, *value);
      }
    }
  }
  if(x264_param_apply_profile(&x264Param, *profile)) {
    puts("profile apply失敗しました。");
    encoder_ = NULL;
  }
  else {
    encoder_ = ttLibC_X264Encoder_makeWithX264ParamT(&x264Param);
  }
#endif
}

X264Encoder::~X264Encoder() {
#ifdef __ENABLE_X264__
  ttLibC_X264Encoder_close(&encoder_);
#endif
}

bool X264Encoder::encodeCallback(void *ptr, ttLibC_H264 *h264) {
  X264Encoder *encoder = (X264Encoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)h264);
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

bool X264Encoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_X264__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_yuv420) {
    puts("yuv420のみ処理可能です。");
    return false;
  }
  return ttLibC_X264Encoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}

bool X264Encoder::forceNextFrameType(std::string type) {
#ifdef __ENABLE_X264__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  ttLibC_X264Encoder_FrameType frameType = X264FrameType_Auto;
  if(type == "I") {
    frameType = X264FrameType_I;
  }
  else if(type == "P") {
    frameType = X264FrameType_P;
  }
  else if(type == "B") {
    frameType = X264FrameType_B;
  }
  else if(type == "IDR") {
    frameType = X264FrameType_IDR;
  }
  else if(type == "Auto") {
    frameType = X264FrameType_Auto;
  }
  else if(type == "Bref") {
    frameType = X264FrameType_Bref;
  }
  else if(type == "KeyFrame") {
    frameType = X264FrameType_KeyFrame;
  }
  else {
    return false;
  }
  return ttLibC_X264Encoder_forceNextFrameType(encoder_, frameType);
#else
  return false;
#endif
}
