#include "x265.h"
#include "../frame.h"
#ifdef __ENABLE_X265__
# include <x265.h>
#endif

X265Encoder::X265Encoder(Local<Object> params) : Encoder() {
  type_ = get_x265;
#ifdef __ENABLE_X265__
  // width height preset tunr profile paramを送る
  uint32_t width  = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t height = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  String::Utf8Value preset( Nan::Get(params, Nan::New("preset").ToLocalChecked()).ToLocalChecked()->ToString());
  String::Utf8Value tune(   Nan::Get(params, Nan::New("tune").ToLocalChecked()).ToLocalChecked()->ToString());
  String::Utf8Value profile(Nan::Get(params, Nan::New("profile").ToLocalChecked()).ToLocalChecked()->ToString());
  Local<Object> param = Nan::Get(params, Nan::New("param").ToLocalChecked()).ToLocalChecked()->ToObject();
  x265_api *api;
  x265_param *x265Param;
  if(!ttLibC_X265Encoder_getDefaultX265ApiAndParam((void **)&api, (void **)&x265Param, *preset, *tune, width, height)) {
    puts("default api paramの取得に失敗しました。");
    return;
  }
  {
    Local<Array> keys = param->GetPropertyNames();
    for(int i = 0, max = keys->Length();i < max;++ i) {
      String::Utf8Value key(keys->Get(i)->ToString());
      String::Utf8Value value(Nan::Get(param, Nan::New(*key).ToLocalChecked()).ToLocalChecked()->ToString());
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

bool X265Encoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_X265__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
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