#ifndef CSRC_DECODER_AVCODEC_H
#define CSRC_DECODER_AVCODEC_H

#include "../decoder.h"
#ifdef __ENABLE_AVCODEC__
# include <ttLibC/decoder/avcodecDecoder.h>
#endif

class AvcodecDecoder : public Decoder {
public:
  AvcodecDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_Frame *frame);
  ~AvcodecDecoder();
#ifdef __ENABLE_AVCODEC__
  ttLibC_AvcodecDecoder *decoder_;
#endif
};

#endif
