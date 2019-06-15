#include "openh264Encoder.h"
#include "../frame.h"
#include "../util.h"
#ifdef __ENABLE_OPENH264__
# include <wels/codec_api.h>
#endif

Openh264Encoder::Openh264Encoder(Local<Object> params) : Encoder() {
  type_ = get_openh264;
#ifdef __ENABLE_OPENH264__
  uint32_t width  = Uint32Value(Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked());
  uint32_t height = Uint32Value(Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked());
  Local<Object> param = ToObject(Nan::Get(params, Nan::New("param").ToLocalChecked()).ToLocalChecked());
  Local<Array> spatialParamArray = Local<Array>::Cast(Nan::Get(params, Nan::New("spatialParamArray").ToLocalChecked()).ToLocalChecked());
  if(width == 0 || height == 0) {
    puts("縦横入力パラメーターが不正です。");
    return;
  }
  SEncParamExt paramExt;
  // width height params spatialParamsArrayが必要
  ttLibC_Openh264Encoder_getDefaultSEncParamExt(&paramExt, width, height);
  {
    Local<Array> keys = param->GetPropertyNames(Nan::GetCurrentContext()).ToLocalChecked();
    for(int i = 0, max = keys->Length();i < max;++ i) {
      String::Utf8Value key(v8::Isolate::GetCurrent(), ToString(keys->Get(i)));
      String::Utf8Value value(v8::Isolate::GetCurrent(), ToString(Nan::Get(param, Nan::New(*key).ToLocalChecked()).ToLocalChecked()));
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
    Local<Object> spatialParam = ToObject(spatialParamArray->Get(i));
    Local<Array> keys = spatialParam->GetPropertyNames(Nan::GetCurrentContext()).ToLocalChecked();
    for(int j = 0, max = keys->Length();j < max;++ j) {
      String::Utf8Value key(v8::Isolate::GetCurrent(), ToString(keys->Get(j)));
      String::Utf8Value value(v8::Isolate::GetCurrent(), ToString(Nan::Get(spatialParam, Nan::New(*key).ToLocalChecked()).ToLocalChecked()));
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

bool Openh264Encoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_OPENH264__
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
  return ttLibC_Openh264Encoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}

bool Openh264Encoder::setRCMode(std::string mode) {
#ifdef __ENABLE_OPENH264__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  ttLibC_Openh264Encoder_RCType type;
	if(mode == "QualityMode") {
		type = Openh264EncoderRCType_QualityMode;
	}
	else if(mode == "BitrateMode") {
		type = Openh264EncoderRCType_BitrateMode;
	}
	else if(mode == "BufferbasedMode") {
		type = Openh264EncoderRCType_BufferbasedMode;
	}
	else if(mode == "TimestampMode") {
		type = Openh264EncoderRCType_TimestampMode;
	}
	else if(mode == "BitrateModePostSkip") {
		type = Openh264EncoderRCType_BitrateModePostSkip;
	}
	else if(mode == "OffMode") {
		type = Openh264EncoderRCType_OffMode;
	}
	else {
		return false;
	}
	return ttLibC_Openh264Encoder_setRCMode(encoder_, type);
#else
  return false;
#endif
}

bool Openh264Encoder::setIDRInterval(uint32_t value) {
#ifdef __ENABLE_OPENH264__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
	return ttLibC_Openh264Encoder_setIDRInterval(encoder_, value);
#else
  return false;
#endif
}

bool Openh264Encoder::forceNextKeyFrame() {
#ifdef __ENABLE_OPENH264__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
	return ttLibC_Openh264Encoder_forceNextKeyFrame(encoder_);
#else
  return false;
#endif
}
