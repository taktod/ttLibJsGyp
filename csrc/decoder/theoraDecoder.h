#ifndef CSRC_DECODER_THEORA_H
#define CSRC_DECODER_THEORA_H

#include "../decoder.h"
#include <ttLibC/frame/video/yuv420.h>
#ifdef __ENABLE_THEORA__
# include <ttLibC/decoder/theoraDecoder.h>
#endif

class TheoraDecoder : public Decoder {
public:
  TheoraDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_Yuv420 *yuv);
  ~TheoraDecoder();
#ifdef __ENABLE_THEORA__
  ttLibC_TheoraDecoder *decoder_;
#endif
};

#endif
