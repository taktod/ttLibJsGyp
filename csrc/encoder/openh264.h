#ifndef CSRC_ENCODER_OPENH264_H
#define CSRC_ENCODER_OPENH264_H

#include "../encoder.h"
#include <ttLibC/frame/video/h264.h>
#ifdef __ENABLE_OPENH264__
# include <ttLibC/encoder/openh264Encoder.h>
#endif

class Openh264Encoder : public Encoder {
public:
  Openh264Encoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_H264 *frame);
  ~Openh264Encoder();
#ifdef __ENABLE_OPENH264__
  ttLibC_Openh264Encoder *encoder_;
#endif
};

#endif
