#include "x265Encoder.h"
#include "../frame.h"
#include "../util.h"
#ifdef __ENABLE_X265__
# include <x265.h>
#endif

X265Encoder::X265Encoder(Local<Object> params) : Encoder() {
  type_ = get_x265;
#ifdef __ENABLE_X265__
  // width height preset tunr profile paramを送る
  uint32_t width  = Uint32Value(Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked());
  uint32_t height = Uint32Value(Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked());
  String::Utf8Value preset( v8::Isolate::GetCurrent(), ToString(Nan::Get(params, Nan::New("preset").ToLocalChecked()).ToLocalChecked()));
  String::Utf8Value tune(   v8::Isolate::GetCurrent(), ToString(Nan::Get(params, Nan::New("tune").ToLocalChecked()).ToLocalChecked()));
  String::Utf8Value profile(v8::Isolate::GetCurrent(), ToString(Nan::Get(params, Nan::New("profile").ToLocalChecked()).ToLocalChecked()));
  Local<Object> param = ToObject(Nan::Get(params, Nan::New("param").ToLocalChecked()).ToLocalChecked());
  x265_api *api;
  x265_param *x265Param;
  if(!ttLibC_X265Encoder_getDefaultX265ApiAndParam((void **)&api, (void **)&x265Param, *preset, *tune, width, height)) {
    puts("default api paramの取得に失敗しました。");
    return;
  }
  {
    Local<Array> keys = param->GetPropertyNames(Nan::GetCurrentContext()).ToLocalChecked();
    for(int i = 0, max = keys->Length();i < max;++ i) {
      String::Utf8Value key(v8::Isolate::GetCurrent(), ToString(keys->Get(i)));
      String::Utf8Value value(v8::Isolate::GetCurrent(), ToString(Nan::Get(param, Nan::New(*key).ToLocalChecked()).ToLocalChecked()));
      int result = x265_param_parse(x265Param, *key, *value);
      if(result < 0) {
        printf("%s %s:パラメーター設定失敗しました。\n", *key, *value);
      }
    }
  }
  if(x265_param_apply_profile(x265Param, *profile)) {
    puts("profile apply失敗しました。");
    return;
  }
  encoder_ = ttLibC_X265Encoder_makeWithX265ApiAndParam((void *)api, (void *)x265Param);
#endif
}

X265Encoder::~X265Encoder() {
#ifdef __ENABLE_X265__
  ttLibC_X265Encoder_close(&encoder_);
#endif
}

bool X265Encoder::encodeCallback(void *ptr, ttLibC_H265 *h265) {
  X265Encoder *encoder = (X265Encoder *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)h265);
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

bool X265Encoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_X265__
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
  return ttLibC_X265Encoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}

bool X265Encoder::forceNextFrameType(std::string type) {
#ifdef __ENABLE_X265__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  ttLibC_X265Encoder_FrameType frameType = X265FrameType_Auto;
  if(type == "I") {
    frameType = X265FrameType_I;
  }
  else if(type == "P") {
    frameType = X265FrameType_P;
  }
  else if(type == "B") {
    frameType = X265FrameType_B;
  }
  else if(type == "IDR") {
    frameType = X265FrameType_IDR;
  }
  else if(type == "Auto") {
    frameType = X265FrameType_Auto;
  }
  else if(type == "Bref") {
    frameType = X265FrameType_Bref;
  }
  else {
    return false;
  }
  return ttLibC_X265Encoder_forceNextFrameType(encoder_, frameType);
#else
  return false;
#endif
  return true;
}
