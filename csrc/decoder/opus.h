#ifndef CSRC_DECODER_OPUS_H
#define CSRC_DECODER_OPUS_H

#include "../decoder.h"
#include <ttLibC/frame/audio/pcmS16.h>
#ifdef __ENABLE_OPUS__
# include <ttLibC/decoder/opusDecoder.h>
#endif

class OpusDecoder : public Decoder {
public:
  OpusDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_PcmS16 *pcm);
  ~OpusDecoder();
#ifdef __ENABLE_OPUS__
  ttLibC_OpusDecoder *decoder_;
#endif
};

#endif
