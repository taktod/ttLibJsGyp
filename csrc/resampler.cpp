#include "predef.h"
#include "resampler.h"
#include "frame.h"
#include "util.h"

#include "resampler/audioResampler.h"
#include "resampler/imageResampler.h"
#include "resampler/libyuvRotateResampler.h"
#include "resampler/libyuvScaleResampler.h"
#include "resampler/soundtouchResampler.h"
#include "resampler/speexdspResampler.h"
#include "resampler/swresampleResampler.h"
#include "resampler/swscaleResampler.h"

#include <string>

class DummyResampler : public Resampler {
public:
  DummyResampler() : Resampler() {}
  bool resample(ttLibC_Frame *frame) {
    return false;
  }
private:
  ~DummyResampler() {}
};

void TTLIBJSGYP_CDECL Resampler::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Resampler").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "resample",          Resample);
  SetPrototypeMethod(tpl, "setRate",           SetRate);
  SetPrototypeMethod(tpl, "setTempo",          SetTempo);
  SetPrototypeMethod(tpl, "setRateChange",     SetRateChange);
  SetPrototypeMethod(tpl, "setTempoChange",    SetTempoChange);
  SetPrototypeMethod(tpl, "setPitch",          SetPitch);
  SetPrototypeMethod(tpl, "setPitchOctaves",   SetPitchOctaves);
  SetPrototypeMethod(tpl, "setPitchSemiTones", SetPitchSemiTones);
  Local<Function> func = Nan::GetFunction(tpl).ToLocalChecked();
  constructor().Reset(func);
  Nan::Set(func, Nan::New("check").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CheckAvailable)).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Resampler").ToLocalChecked(),
    func);
}

static bool checkAvailable(std::string type) {
  bool result = false;
  if(type == "audio") {
    result = true;
  }
  else if(type == "image") {
    result = true;
  }
  else if(type == "libyuvscale") {
#ifdef __ENABLE_LIBYUV__
    result = true;
#endif
  }
  else if(type == "libyuvrotate") {
#ifdef __ENABLE_LIBYUV__
    result = true;
#endif
  }
  else if(type == "soundtouch") {
#ifdef __ENABLE_SOUNDTOUCH__
    result = true;
#endif
  }
  else if(type == "speexdsp") {
#ifdef __ENABLE_SPEEXDSP__
    result = true;
#endif
  }
  else if(type == "swresample") {
#ifdef __ENABLE_SWRESAMPLE__
    result = true;
#endif
  }
  else if(type == "swscale") {
#ifdef __ENABLE_SWSCALE__
    result = true;
#endif
  }
  return result;
}

NAN_METHOD(Resampler::CheckAvailable) {
  bool result = false;
  if(info.Length() > 0) {
    std::string type(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0])));
    result = checkAvailable(type);
  }
  info.GetReturnValue().Set(result);
}
NAN_METHOD(Resampler::New) {
  std::string type(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0])));
  if(!checkAvailable(type)) {
    Nan::ThrowError(Nan::New(type + " resampler is not available.").ToLocalChecked());
    return;
  }
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    Resampler *resampler = NULL;
    if(type == "audio") {
      resampler = new AudioResampler(ToObject(info[1]));
    }
    else if(type == "image") {
      resampler = new ImageResampler(ToObject(info[1]));
    }
    else if(type == "libyuvrotate") {
      resampler = new LibyuvRotateResampler(ToObject(info[1]));
    }
    else if(type == "libyuvscale") {
      resampler = new LibyuvScaleResampler(ToObject(info[1]));
    }
    else if(type == "soundtouch") {
      resampler = new SoundtouchResampler(ToObject(info[1]));
    }
    else if(type == "speexdsp") {
      resampler = new SpeexdspResampler(ToObject(info[1]));
    }
    else if(type == "swresample") {
      resampler = new SwresampleResampler(ToObject(info[1]));
    }
    else if(type == "swscale") {
      resampler = new SwscaleResampler(ToObject(info[1]));
    }
    else {
      printf("%sは未定義です。\n", type.c_str());
      resampler = new DummyResampler();
    }
    if(resampler != NULL) {
      resampler->Wrap(info.This());
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

NAN_METHOD(Resampler::Resample) {
  if(info.Length() == 2) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler == NULL) {
      puts("resamplerがありません、");
      info.GetReturnValue().Set(false);
      return;
    }
    resampler->callback_ = info[1];
    info.GetReturnValue().Set(
      resampler->resample(Frame::refFrame(info[0]))
    );
    return;
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Resampler::SetRate) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setRate(NumberValue(info[0]));
    }
  }
}

NAN_METHOD(Resampler::SetTempo) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setTempo(NumberValue(info[0]));
    }
  }
}

NAN_METHOD(Resampler::SetRateChange) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setRateChange(NumberValue(info[0]));
    }
  }
}

NAN_METHOD(Resampler::SetTempoChange) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setTempoChange(NumberValue(info[0]));
    }
  }
}

NAN_METHOD(Resampler::SetPitch) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setPitch(NumberValue(info[0]));
    }
  }
}

NAN_METHOD(Resampler::SetPitchOctaves) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setPitchOctaves(NumberValue(info[0]));
    }
  }
}

NAN_METHOD(Resampler::SetPitchSemiTones) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setPitchSemiTones(NumberValue(info[0]));
    }
  }
}

Resampler::Resampler() {
  jsFrame_.Reset(Frame::newInstance());
}
