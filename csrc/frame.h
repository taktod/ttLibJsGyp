#ifndef CSRC_FRAME_H
#define CSRC_FRAME_H

#include <nan.h>
#include <ttLibC/frame/frame.h>
#include <string>

using namespace v8;

class Frame : public Nan::ObjectWrap {
public:
  static void              classInit(Local<Object> target);
  static Local<Object>     newInstance();
  static bool              setFrame(Local<Object> jsFrame, ttLibC_Frame *frame);
  static ttLibC_Frame_Type getFrameType(std::string name);
  static ttLibC_Frame     *refFrame(Local<Value> jsFrame); // このrefFrameが必要な情報を復元する動作みたいですね。
private:
  static ttLibC_Frame *restoreTtLibCFrame(
      ttLibC_Frame  *prev_frame,
      Local<Value>   binary,
      Local<Object>  params);
  static NAN_METHOD(New);
  static NAN_METHOD(GetBinaryBuffer);
  // このfromBinaryBufferの動作はbufferにstaticで実行するより、該当フレームから復元する方がしっくりきそう。
  // というわけでやってみようと思う。
  // まぁ、すでにttLibJsGyp2に本家のやつあるんで問題ないだろう。
  static NAN_METHOD(FromBinaryBuffer);
  static NAN_METHOD(Clone);
  static NAN_METHOD(Restore);
  static NAN_METHOD(Copy); // copyは基本撤去したい

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  explicit Frame();
  ~Frame();

  ttLibC_Frame *frame_;
  bool          isRef_;
};

#endif
