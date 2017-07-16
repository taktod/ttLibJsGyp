#ifndef CSRC_WRITER_H
#define CSRC_WRITER_H

#include <nan.h>
#include <ttLibC/container/container.h>
#include <ttLibC/container/flv.h>
#include <ttLibC/container/mkv.h>
#include <ttLibC/container/mp4.h>
#include <ttLibC/container/mpegts.h>

using namespace v8;

// これ・・・分割動作の調整が必要になるのか・・・
// setModeとwriteInfo(mpegtsで必要)の実装が必要なわけか・・・
class Writer : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
private:
  static NAN_METHOD(New);
  static NAN_METHOD(WriteFrame);
  static NAN_METHOD(SetMode);
  static NAN_METHOD(WriteInfo);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  explicit Writer(Nan::NAN_METHOD_ARGS_TYPE info);
  ~Writer();
  static bool writeCallback(void *ptr, void *data, size_t data_size);
  
  ttLibC_ContainerWriter *writer_;
  Local<Value> callback_;
};

#endif
