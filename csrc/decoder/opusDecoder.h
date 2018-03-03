#ifndef CSRC_DECODER_OPUS_H
#define CSRC_DECODER_OPUS_H

#include "../decoder.h"
#include <ttLibC/frame/audio/pcms16.h>
#ifdef __ENABLE_OPUS__
# include <ttLibC/decoder/opusDecoder.h>
#endif

class OpusDecoder_ : public Decoder {
public:
  OpusDecoder_(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
  int  codecControl(std::string control, int value);
private:
  static bool decodeCallback(void *ptr, ttLibC_PcmS16 *pcm);
  ~OpusDecoder_();
#ifdef __ENABLE_OPUS__
  ttLibC_OpusDecoder *decoder_;
#endif
};

#endif
