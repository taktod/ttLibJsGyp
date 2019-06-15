#include "predef.h"
#include "encoder.h"
#include "frame.h"
#include "util.h"

#include "encoder/audioConverterEncoder.h"
#include "encoder/faacEncoder.h"
#include "encoder/fdkaacEncoder.h"
#include "encoder/mp3lameEncoder.h"
#include "encoder/jpegEncoder.h"
#include "encoder/openh264Encoder.h"
#include "encoder/opusEncoder.h"
#include "encoder/speexEncoder.h"
#include "encoder/theoraEncoder.h"
#include "encoder/vorbisEncoder.h"
#include "encoder/vtCompressSessionEncoder.h"
#include "encoder/x264Encoder.h"
#include "encoder/x265Encoder.h"
#include "encoder/msAacEncoder.h"
#include "encoder/msH264Encoder.h"

#include <string>

class DummyEncoder : public Encoder {
public:
  DummyEncoder() : Encoder() {}
  bool encode(ttLibC_Frame *frame) {
    return false;
  }
private:
  ~DummyEncoder() {}
};

void TTLIBJSGYP_CDECL Encoder::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Encoder").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "encode",             Encode);
  SetPrototypeMethod(tpl, "forceNextFrameType", ForceNextFrameType);
  SetPrototypeMethod(tpl, "setQuality",         SetQuality);
  SetPrototypeMethod(tpl, "setRCMode",          SetRCMode);
  SetPrototypeMethod(tpl, "setIDRInterval",     SetIDRInterval);
  SetPrototypeMethod(tpl, "forceNextKeyFrame",  ForceNextKeyFrame);
  SetPrototypeMethod(tpl, "setBitrate",         SetBitrate);
  SetPrototypeMethod(tpl, "setComplexity",      SetComplexity);
  SetPrototypeMethod(tpl, "setCodecControl",    SetCodecControl);
  SetPrototypeMethod(tpl, "getCodecControl",    GetCodecControl);
  Local<Function> func = Nan::GetFunction(tpl).ToLocalChecked();
  constructor().Reset(func);
  Nan::Set(func, Nan::New("check").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CheckAvailable)).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Encoder").ToLocalChecked(),
    func);
  MSH264Encoder::classInit(target);
}

static bool checkAvailable(std::string type) {
  bool result = false;
  if(type == "audioConverter") {
#ifdef __ENABLE_APPLE__
    result = true;
#endif
  }
  else if(type == "faac") {
#ifdef __ENABLE_FAAC_ENCODE__
    result = true;
#endif
  }
  else if(type == "fdkaac") {
#ifdef __ENABLE_FDKAAC_ENCODE__
    result = true;
#endif
  }
  else if(type == "jpeg") {
#ifdef __ENABLE_JPEG__
    result = true;
#endif
  }
  else if(type == "mp3lame") {
#ifdef __ENABLE_MP3LAME_ENCODE__
    result = true;
#endif
  }
  else if(type == "msAac") {
#ifdef __ENABLE_WIN32__
    result = true;
#endif
  }
  else if(type == "msH264") {
#ifdef __ENABLE_WIN32__
    result = true;
#endif
  }
  else if(type == "openh264") {
#ifdef __ENABLE_OPENH264__
    result = true;
#endif
  }
  else if(type == "opus") {
#ifdef __ENABLE_OPUS__
    result = true;
#endif
  }
  else if(type == "speex") {
#ifdef __ENABLE_SPEEX__
    result = true;
#endif
  }
  else if(type == "theora") {
#ifdef __ENABLE_THEORA__
    result = true;
#endif
  }
  else if(type == "vorbis") {
#ifdef __ENABLE_VORBIS_ENCODE__
    result = true;
#endif
  }
  else if(type == "vtCompressSession") {
#ifdef __ENABLE_APPLE__
    result = true;
#endif
  }
  else if(type == "x264") {
#ifdef __ENABLE_X264__
    result = true;
#endif
  }
  else if(type == "x265") {
#ifdef __ENABLE_X265__
    result = true;
#endif
  }
  return result;
}

NAN_METHOD(Encoder::CheckAvailable) {
  bool result = false;
  if(info.Length() > 0) {
    std::string type(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0])));
    result = checkAvailable(type);
  }
  info.GetReturnValue().Set(result);
}

NAN_METHOD(Encoder::New) {
  std::string type(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0])));
  if(!checkAvailable(type)) {
    Nan::ThrowError(Nan::New(type + " encoder is not available.").ToLocalChecked());
    return;
  }
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    Encoder *encoder = NULL;
    if(type == "audioConverter") {
      encoder = new AudioConverterEncoder(ToObject(info[1]));
    }
    else if(type == "faac") {
      encoder = new FaacEncoder(ToObject(info[1]));
    }
    else if(type == "fdkaac") {
      encoder = new FdkaacEncoder(ToObject(info[1]));
    }
    else if(type == "jpeg") {
      encoder = new JpegEncoder(ToObject(info[1]));
    }
    else if(type == "mp3lame") {
      encoder = new Mp3lameEncoder(ToObject(info[1]));
    }
    else if(type == "msAac") {
      encoder = new MSAacEncoder(ToObject(info[1]));
    }
    else if(type == "msH264") {
      encoder = new MSH264Encoder(ToObject(info[1]));
    }
    else if(type == "openh264") {
      encoder = new Openh264Encoder(ToObject(info[1]));
    }
    else if(type == "opus") {
      encoder = new OpusEncoder_(ToObject(info[1]));
    }
    else if(type == "speex") {
      encoder = new SpeexEncoder(ToObject(info[1]));
    }
    else if(type == "theora") {
      encoder = new TheoraEncoder(ToObject(info[1]));
    }
    else if(type == "vorbis") {
      encoder = new VorbisEncoder(ToObject(info[1]));
    }
    else if(type == "vtCompressSession") {
      encoder = new VtCompressSessionEncoder(ToObject(info[1]));
    }
    else if(type == "x264") {
      encoder = new X264Encoder(ToObject(info[1]));
    }
    else if(type == "x265") {
      encoder = new X265Encoder(ToObject(info[1]));
    }
    else {
      printf("%sは未定義です。\n", type.c_str());
      encoder = new DummyEncoder();
    }
    // TODO ここでdummy wrapperをつくっておかないと
    // unwrapしたときにゴミメモリーが応答されることがあるみたいですね。
    if(encoder != NULL) {
      encoder->Wrap(info.This());
    }
    info.GetReturnValue().Set(info.This());
  }
  else {
    Local<Value> *argv = new Local<Value>[info.Length()];
    for(int i = 0;i < info.Length();++ i) {
      argv[i] = info[i];
    }
    Local<Function> cons = Nan::New(constructor());
    info.GetReturnValue().Set(Nan::NewInstance(cons, (const int)info.Length(), argv).ToLocalChecked());
    delete[] argv;
  }
}

NAN_METHOD(Encoder::Encode) {
  if(info.Length() == 2) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    encoder->callback_ = info[1];
    info.GetReturnValue().Set(
      encoder->encode(Frame::refFrame(info[0]))
    );
    return;
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::ForceNextFrameType) {
  if(info.Length() == 1) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    switch(encoder->type_) {
    case get_x264:
      {
        X264Encoder *x264Encoder = (X264Encoder *)encoder;
        info.GetReturnValue().Set(x264Encoder->forceNextFrameType(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0]))));
      }
      return;
    case get_x265:
      {
        X265Encoder *x265Encoder = (X265Encoder *)encoder;
        info.GetReturnValue().Set(x265Encoder->forceNextFrameType(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0]))));
      }
      return;
    default:
      break;
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::SetQuality) {
  if(info.Length() == 1) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(encoder->type_ == get_jpeg) {
      JpegEncoder *jpegEncoder = (JpegEncoder *)encoder;
      info.GetReturnValue().Set(jpegEncoder->setQuality(Uint32Value(info[0])));
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::SetRCMode) {
  if(info.Length() == 1) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(encoder->type_ == get_openh264) {
      Openh264Encoder *openh264Encoder = (Openh264Encoder *)encoder;
      info.GetReturnValue().Set(openh264Encoder->setRCMode(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0]))));
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::SetIDRInterval) {
  if(info.Length() == 1) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(encoder->type_ == get_openh264) {
      Openh264Encoder *openh264Encoder = (Openh264Encoder *)encoder;
      info.GetReturnValue().Set(openh264Encoder->setIDRInterval(Uint32Value(info[0])));
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::ForceNextKeyFrame) {
  if(info.Length() == 0) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(encoder->type_ == get_openh264) {
      Openh264Encoder *openh264Encoder = (Openh264Encoder *)encoder;
      info.GetReturnValue().Set(openh264Encoder->forceNextKeyFrame());
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::SetBitrate) {
  if(info.Length() == 1) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    switch(encoder->type_) {
    case get_opus:
      {
        OpusEncoder_ *opusEncoder = (OpusEncoder_ *)encoder;
        info.GetReturnValue().Set(opusEncoder->setBitrate(Uint32Value(info[0])));
      }
      return;
    case get_fdkaac:
      {
        FdkaacEncoder *fdkaacEncoder = (FdkaacEncoder *)encoder;
        info.GetReturnValue().Set(fdkaacEncoder->setBitrate(Uint32Value(info[0])));
      }
      return;
    default:
      break;
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::SetComplexity) {
  if(info.Length() == 1) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(encoder->type_ == get_opus) {
      OpusEncoder_ *opusEncoder = (OpusEncoder_ *)encoder;
      info.GetReturnValue().Set(opusEncoder->setComplexity(Uint32Value(info[0])));
      return;
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::SetCodecControl) {
  if(info.Length() == 2) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(encoder->type_ == get_opus) {
      OpusEncoder_ *opusEncoder = (OpusEncoder_ *)encoder;
      info.GetReturnValue().Set(opusEncoder->codecControl(
        std::string(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0]))),
        Uint32Value(info[1])));
      return;
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Encoder::GetCodecControl) {
  if(info.Length() == 1) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(encoder->type_ == get_opus) {
      OpusEncoder_ *opusEncoder = (OpusEncoder_ *)encoder;
      info.GetReturnValue().Set(opusEncoder->codecControl(
        std::string(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0]))),
        0));
      return;
    }
  }
  info.GetReturnValue().Set(false);
}

Encoder::Encoder() {
  jsFrame_.Reset(Frame::newInstance());
}
