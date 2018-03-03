#ifndef CSRC_ENCODER_OPUS_H
#define CSRC_ENCODER_OPUS_H

#include "../encoder.h"
#include <ttLibC/frame/audio/opus.h>
#ifdef __ENABLE_OPUS__
# include <ttLibC/encoder/opusEncoder.h>
#endif

class OpusEncoder_ : public Encoder {
public:
  OpusEncoder_(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
  bool setBitrate(uint32_t value);
  bool setComplexity(uint32_t value);
  int  codecControl(std::string control, int value);
private:
  static bool encodeCallback(void *ptr, ttLibC_Opus *frame);
  ~OpusEncoder_();
#ifdef __ENABLE_OPUS__
  ttLibC_OpusEncoder *encoder_;
#endif
};

#endif
