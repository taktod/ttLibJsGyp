#ifndef CSRC_DECODER_LIBPNG_H
#define CSRC_DECODER_LIBPNG_H

#include "../decoder.h"
#include <ttLibC/frame/video/bgr.h>
#ifdef __ENABLE_LIBPNG__
# include <ttLibC/decoder/pngDecoder.h>
#endif

class PngDecoder : public Decoder {
public:
  PngDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_Bgr *bgr);
  ~PngDecoder();
#ifdef __ENABLE_LIBPNG__
  ttLibC_PngDecoder *decoder_;
#endif
};

#endif
