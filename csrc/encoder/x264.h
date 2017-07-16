#ifndef CSRC_ENCODER_X264_H
#define CSRC_ENCODER_X264_H

#include "../encoder.h"
#include <ttLibC/frame/video/h264.h>
#ifdef __ENABLE_X264__
# include <ttLibC/encoder/x264Encoder.h>
#endif

class X264Encoder : public Encoder {
public:
  X264Encoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_H264 *frame);
  ~X264Encoder();
#ifdef __ENABLE_X264__
  ttLibC_X264Encoder *encoder_;
#endif
};

#endif