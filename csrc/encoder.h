#ifndef CSRC_ENCODER_H
#define CSRC_ENCODER_H

#include <nan.h>
#include <ttLibC/frame/frame.h>

using namespace v8;

typedef enum gypEncoderType_e {
  get_audioConverter,
  get_avcodec,
  get_faac,
  get_fdkaac,
  get_jpeg,
  get_mp3lame,
  get_openh264,
  get_opus,
  get_speex,
  get_theora,
  get_vorbis,
  get_vtCompressSession,
  get_x264,
  get_x265,
  get_msAac,
  get_msH264
} gypEncoderType_e;

class Encoder : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
private:
  static NAN_METHOD(CheckAvailable);

  static NAN_METHOD(New);
  static NAN_METHOD(Encode);
  static NAN_METHOD(ForceNextFrameType);
  static NAN_METHOD(SetRCMode);
  static NAN_METHOD(SetIDRInterval);
  static NAN_METHOD(ForceNextKeyFrame);
  static NAN_METHOD(SetBitrate);
  static NAN_METHOD(SetComplexity);
  static NAN_METHOD(SetCodecControl);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
protected:
  Encoder();
  virtual ~Encoder() {};
  virtual bool encode(ttLibC_Frame *frame) = 0;
  gypEncoderType_e        type_;
  Nan::Persistent<Object> jsFrame_;
  Local<Value>            callback_;
};

#endif
