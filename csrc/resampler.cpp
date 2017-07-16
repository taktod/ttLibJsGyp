#include "predef.h"
#include "resampler.h"
#include "frame.h"

#include "resampler/audio.h"
#include "resampler/image.h"
#include "resampler/soundtouch.h"
#include "resampler/speexdsp.h"

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

NAN_METHOD(Resampler::CheckAvailable) {
  bool result = false;
  if(info.Length() > 0) {
    std::string type(*String::Utf8Value(info[0]->ToString()));
    if(type == "audio") {
      result = true;
    }
    else if(type == "image") {
      result = true;
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
  }
  info.GetReturnValue().Set(result);
}
NAN_METHOD(Resampler::New) {
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    std::string type(*String::Utf8Value(info[0]->ToString()));
    Resampler *resampler = NULL;
    if(type == "audio") {
      resampler = new AudioResampler(info[1]->ToObject());
    }
    else if(type == "image") {
      resampler = new ImageResampler(info[1]->ToObject());
    }
    else if(type == "soundtouch") {
      resampler = new SoundtouchResampler(info[1]->ToObject());
    }
    else if(type == "speexdsp") {
      resampler = new SpeexdspResampler(info[1]->ToObject());
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
      soundtouch->setRate(info[0]->NumberValue());
    }
  }
}

NAN_METHOD(Resampler::SetTempo) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setTempo(info[0]->NumberValue());
    }
  }
}

NAN_METHOD(Resampler::SetRateChange) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setRateChange(info[0]->NumberValue());
    }
  }
}

NAN_METHOD(Resampler::SetTempoChange) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setTempoChange(info[0]->NumberValue());
    }
  }
}

NAN_METHOD(Resampler::SetPitch) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setPitch(info[0]->NumberValue());
    }
  }
}

NAN_METHOD(Resampler::SetPitchOctaves) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setPitchOctaves(info[0]->NumberValue());
    }
  }
}

NAN_METHOD(Resampler::SetPitchSemiTones) {
  if(info.Length() == 1) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler->type_ == grt_soundtouch) {
      SoundtouchResampler *soundtouch = (SoundtouchResampler *)resampler;
      soundtouch->setPitchSemiTones(info[0]->NumberValue());
    }
  }
}

Resampler::Resampler() {
  jsFrame_.Reset(Frame::newInstance());
}
