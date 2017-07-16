#include "openh264.h"
#include "../frame.h"
#ifdef __ENABLE_OPENH264__
# include <wels/codec_api.h>
#endif

Openh264Encoder::Openh264Encoder(Local<Object> params) : Encoder() {
  type_ = get_openh264;
#ifdef __ENABLE_OPENH264__
  uint32_t width  = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t height = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  Local<Object> param = Nan::Get(params, Nan::New("param").ToLocalChecked()).ToLocalChecked()->ToObject();
  Local<Array> spatialParamArray = Local<Array>::Cast(Nan::Get(params, Nan::New("spatialParamArray").ToLocalChecked()).ToLocalChecked());
  if(width == 0 || height == 0) {
    puts("縦横入力パラメーターが不正です。");
    return;
  }
  SEncParamExt paramExt;
  // width height params spatialParamsArrayが必要
  ttLibC_Openh264Encoder_getDefaultSEncParamExt(&paramExt, width, height);
  {
    Local<Array> keys = param->GetPropertyNames();
    for(int i = 0, max = keys->Length();i < max;++ i) {
      String::Utf8Value key(keys->Get(i)->ToString());
      String::Utf8Value value(Nan::Get(param, Nan::New(*key).ToLocalChecked()).ToLocalChecked()->ToString());
      // あとはデータにアクセスして処理すればよい。
      bool result = ttLibC_Openh264Encoder_paramParse(&paramExt, *key, *value);
      if(!result) {
        printf("%s %s:パラメーター設定失敗しました。\n", *key, *value);
      }
    }
  }
  int num = spatialParamArray->Length();
  if(num > 4) {
    num = 4;
  }
  for(int i = 0;i < num;++ i) {
    Local<Object> spatialParam = spatialParamArray->Get(i)->ToObject();
    Local<Array> keys = spatialParam->GetPropertyNames();
    for(int j = 0, max = keys->Length();j < max;++ j) {
      String::Utf8Value key(keys->Get(j)->ToString());
      String::Utf8Value value(Nan::Get(spatialParam, Nan::New(*key).ToLocalChecked()).ToLocalChecked()->ToString());
      bool result = ttLibC_Openh264Encoder_spatialParamParse(&paramExt, i, *key, *value);
      if(!result) {
        printf("%s %s:パラメーター設定失敗しました。\n", *key, *value);
      }
    }
  }
  encoder_ = ttLibC_Openh264Encoder_makeWithSEncParamExt(&paramExt);
#endif
}

Openh264Encoder::~Openh264Encoder() {
#ifdef __ENABLE_OPENH264__
  ttLibC_Openh264Encoder_close(&encoder_);
#endif
}

bool Openh264Encoder::encodeCallback(void *ptr, ttLibC_H264 *h264) {
  Openh264Encoder *encoder = (Openh264Encoder *)ptr;
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

bool Openh264Encoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_OPENH264__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame->type != frameType_yuv420) {
    puts("yuv420のみ処理可能です。");
    return false;
  }
  return ttLibC_Openh264Encoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}