#ifndef CSRC_ENCODER_JPEG_H
#define CSRC_ENCODER_JPEG_H

#include "../encoder.h"
#include <ttLibC/frame/video/jpeg.h>
#ifdef __ENABLE_JPEG__
# include <ttLibC/encoder/jpegEncoder.h>
#endif

class JpegEncoder : public Encoder {
public:
  JpegEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Jpeg *frame);
  ~JpegEncoder();
#ifdef __ENABLE_JPEG__
  ttLibC_JpegEncoder *encoder_;
#endif
};

#endif