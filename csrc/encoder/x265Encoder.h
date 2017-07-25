#ifndef CSRC_ENCODER_X265_H
#define CSRC_ENCODER_X265_H

#include "../encoder.h"
#include <ttLibC/frame/video/h265.h>
#ifdef __ENABLE_X265__
# include <ttLibC/encoder/x265Encoder.h>
#endif

class X265Encoder : public Encoder {
public:
  X265Encoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_H265 *frame);
  ~X265Encoder();
#ifdef __ENABLE_X265__
  ttLibC_X265Encoder *encoder_;
#endif
};

#endif