#ifndef CSRC_RESAMPLER_H
#define CSRC_RESAMPLER_H

#include <nan.h>
#include <ttLibC/frame/frame.h>

using namespace v8;

typedef enum gypResamplerType_e {
  grt_audio,
  grt_image,
  grt_imageResizer,
  grt_soundtouch,
  grt_speexdsp
} gypResamplerType_e;

class Resampler : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
private:
  static NAN_METHOD(CheckAvailable);

  static NAN_METHOD(New);
  static NAN_METHOD(Resample);
  static NAN_METHOD(SetRate);
  static NAN_METHOD(SetTempo);
  static NAN_METHOD(SetRateChange);
  static NAN_METHOD(SetTempoChange);
  static NAN_METHOD(SetPitch);
  static NAN_METHOD(SetPitchOctaves);
  static NAN_METHOD(SetPitchSemiTones);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
protected:
  Resampler();
  virtual ~Resampler() {};
  virtual bool resample(ttLibC_Frame *frame) = 0;
  gypResamplerType_e      type_;
  Nan::Persistent<Object> jsFrame_;
  Local<Value>            callback_;
};

#endif
