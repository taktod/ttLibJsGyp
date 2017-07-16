#include "x264.h"
#include "../frame.h"
#ifdef __ENABLE_X264__
# include <x264.h>
#endif

X264Encoder::X264Encoder(Local<Object> params) : Encoder() {
  type_ = get_x264;
#ifdef __ENABLE_X264__
  // width height preset tunr profile paramを送る
  uint32_t width  = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t height = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  String::Utf8Value preset( Nan::Get(params, Nan::New("preset").ToLocalChecked()).ToLocalChecked()->ToString());
  String::Utf8Value tune(   Nan::Get(params, Nan::New("tune").ToLocalChecked()).ToLocalChecked()->ToString());
  String::Utf8Value profile(Nan::Get(params, Nan::New("profile").ToLocalChecked()).ToLocalChecked()->ToString());
  Local<Object> param = Nan::Get(params, Nan::New("param").ToLocalChecked()).ToLocalChecked()->ToObject();
  x264_param_t x264Param;
  ttLibC_X264Encoder_getDefaultX264ParamTWithPresetTune(
    &x264Param,
    width,
    height,
    *preset,
    *tune);
  {
    Local<Array> keys = param->GetPropertyNames();
    for(int i = 0, max = keys->Length();i < max;++ i) {
      String::Utf8Value key(keys->Get(i)->ToString());
      String::Utf8Value value(Nan::Get(param, Nan::New(*key).ToLocalChecked()).ToLocalChecked()->ToString());
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

bool X264Encoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_X264__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
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