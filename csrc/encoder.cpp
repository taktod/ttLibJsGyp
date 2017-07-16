#include "predef.h"
#include "encoder.h"
#include "frame.h"

#include "encoder/audioConverter.h"
#include "encoder/faac.h"
#include "encoder/mp3lame.h"
#include "encoder/jpeg.h"
#include "encoder/openh264.h"
#include "encoder/opus.h"
#include "encoder/theora.h"
#include "encoder/vtCompressSession.h"
#include "encoder/x264.h"
#include "encoder/x265.h"
#include "encoder/msAac.h"
#include "encoder/msH264.h"

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
  SetPrototypeMethod(tpl, "setRCMode",          SetRCMode);
  SetPrototypeMethod(tpl, "setIDRInterval",     SetIDRInterval);
  SetPrototypeMethod(tpl, "forceNextKeyFrame",  ForceNextKeyFrame);
  SetPrototypeMethod(tpl, "setBitrate",         SetBitrate);
  SetPrototypeMethod(tpl, "setComplexity",      SetComplexity);
  SetPrototypeMethod(tpl, "setCodecControl",    SetCodecControl);
  Local<Function> func = Nan::GetFunction(tpl).ToLocalChecked();
  constructor().Reset(func);
  Nan::Set(func, Nan::New("check").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CheckAvailable)).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Encoder").ToLocalChecked(),
    func);
  MSH264Encoder::classInit(target);
}

NAN_METHOD(Encoder::CheckAvailable) {
  bool result = false;
  if(info.Length() > 0) {
    std::string type(*String::Utf8Value(info[0]->ToString()));
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
    else if(type == "theora") {
#ifdef __ENABLE_THEORA__
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
  }
  info.GetReturnValue().Set(result);
}

NAN_METHOD(Encoder::New) {
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    std::string type(*String::Utf8Value(info[0]->ToString()));
    Encoder *encoder = NULL;
    if(type == "audioConverter") {
      encoder = new AudioConverterEncoder(info[1]->ToObject());
    }
    else if(type == "faac") {
      encoder = new FaacEncoder(info[1]->ToObject());
    }
    else if(type == "jpeg") {
      encoder = new JpegEncoder(info[1]->ToObject());
    }
    else if(type == "mp3lame") {
      encoder = new Mp3lameEncoder(info[1]->ToObject());
    }
    else if(type == "msAac") {
      encoder = new MSAacEncoder(info[1]->ToObject());
    }
    else if(type == "msH264") {
      encoder = new MSH264Encoder(info[1]->ToObject());
    }
    else if(type == "openh264") {
      encoder = new Openh264Encoder(info[1]->ToObject());
    }
    else if(type == "opus") {
      encoder = new OpusEncoder(info[1]->ToObject());
    }
    else if(type == "theora") {
      encoder = new TheoraEncoder(info[1]->ToObject());
    }
    else if(type == "vtCompressSession") {
      encoder = new VtCompressSessionEncoder(info[1]->ToObject());
    }
    else if(type == "x264") {
      encoder = new X264Encoder(info[1]->ToObject());
    }
    else if(type == "x265") {
      encoder = new X265Encoder(info[1]->ToObject());
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
}

NAN_METHOD(Encoder::ForceNextFrameType) {

}

NAN_METHOD(Encoder::SetRCMode) {

}

NAN_METHOD(Encoder::SetIDRInterval) {

}

NAN_METHOD(Encoder::ForceNextKeyFrame) {

}

NAN_METHOD(Encoder::SetBitrate) {

}

NAN_METHOD(Encoder::SetComplexity) {

}

NAN_METHOD(Encoder::SetCodecControl) {

}

Encoder::Encoder() {
  jsFrame_.Reset(Frame::newInstance());
}
