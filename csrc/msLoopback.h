#ifndef CSRC_LOOPBACK_H
#define CSRC_LOOPBACK_H

#include <nan.h>
#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/util/mmAudioLoopbackUtil.h>

using namespace v8;

class MSLoopback : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
private:
  static NAN_METHOD(New);
  static NAN_METHOD(ListDevice);
  static NAN_METHOD(QueryFrame);
  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  static bool getDeviceCallback(void *ptr, const char *name);
  static bool captureCallback(void *ptr, ttLibC_PcmS16 *pcm);
  MSLoopback(Nan::NAN_METHOD_ARGS_TYPE info);
  ~MSLoopback();

#ifdef __ENABLE_WIN32__
  ttLibC_MmAudioLoopback *loopback_;
#endif
  Nan::Persistent<Object> jsPcmFrame_;
  Local<Value>            callback_;
};

#endif
