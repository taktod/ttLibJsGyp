#ifndef CSRC_DECODER_OPENH264_H
#define CSRC_DECODER_OPENH264_H

#include "../decoder.h"
#include <ttLibC/frame/video/yuv420.h>
#ifdef __ENABLE_OPENH264__
# include <ttLibC/decoder/openh264Decoder.h>
#endif

class Openh264Decoder : public Decoder {
public:
  Openh264Decoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_Yuv420 *yuv);
  ~Openh264Decoder();
#ifdef __ENABLE_OPENH264__
  ttLibC_Openh264Decoder *decoder_;
#endif
};

#endif
