#ifndef CSRC_DECODER_JPEG_H
#define CSRC_DECODER_JPEG_H

#include "../decoder.h"
#include <ttLibC/frame/video/yuv420.h>
#ifdef __ENABLE_JPEG__
# include <ttLibC/decoder/jpegDecoder.h>
#endif

class JpegDecoder : public Decoder {
public:
  JpegDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_Yuv420 *yuv);
  ~JpegDecoder();
#ifdef __ENABLE_JPEG__
  ttLibC_JpegDecoder *decoder_;
#endif
};

#endif
