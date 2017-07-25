#ifndef CSRC_ENCODER_OPUS_H
#define CSRC_ENCODER_OPUS_H

#include "../encoder.h"
#include <ttLibC/frame/audio/opus.h>
#ifdef __ENABLE_OPUS__
# include <ttLibC/encoder/opusEncoder.h>
#endif

class OpusEncoder : public Encoder {
public:
  OpusEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Opus *frame);
  ~OpusEncoder();
#ifdef __ENABLE_OPUS__
  ttLibC_OpusEncoder *encoder_;
#endif
};

#endif