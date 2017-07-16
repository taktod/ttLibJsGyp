#ifndef CSRC_DECODER_H
#define CSRC_DECODER_H

#include <nan.h>
#include <ttLibC/frame/frame.h>

using namespace v8;

typedef enum gypDecoderType_e {
  gdt_audioConverter,
  gdt_avcodec,
  gdt_jpeg,
  gdt_mp3lame,
  gdt_openh264,
  gdt_opus,
  gdt_speex,
  gdt_theora,
  gdt_vorbis,
  gdt_vtDecompressSession
} gypDecoderType_e;

class Decoder : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
private:
  static NAN_METHOD(CheckAvailable);

  static NAN_METHOD(New);
  static NAN_METHOD(Decode);
  static NAN_METHOD(SetCodecControl);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
protected:
  Decoder();
  virtual ~Decoder() {};
  virtual bool decode(ttLibC_Frame *frame) = 0;
  gypDecoderType_e        type_;
  Nan::Persistent<Object> jsFrame_;
  Local<Value>            callback_;
};

#endif
