#ifndef CSRC_READER_H
#define CSRC_READER_H

#include <nan.h>
#include <ttLibC/container/container.h>
#include <ttLibC/container/flv.h>
#include <ttLibC/container/mkv.h>
#include <ttLibC/container/mp4.h>
#include <ttLibC/container/mpegts.h>

using namespace v8;

class Reader : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
private:
  static NAN_METHOD(New);
  static NAN_METHOD(ReadFrame);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  explicit Reader(Nan::NAN_METHOD_ARGS_TYPE info);
  ~Reader();
  static bool frameCallback(void *ptr, ttLibC_Frame *frame);
  static bool flvCallback(void *ptr, ttLibC_Flv *flv);
  static bool mkvCallback(void *ptr, ttLibC_Mkv *mkv);
  static bool mp4Callback(void *ptr, ttLibC_Mp4 *mp4);
  static bool mpegtsCallback(void *ptr, ttLibC_Mpegts *mpegts);

  ttLibC_ContainerReader *reader_;
  Nan::Persistent<Object> jsVideoFrame_;
  Nan::Persistent<Object> jsAudioFrame_;
  Local<Value>            callback_;
};

#endif
