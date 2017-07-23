#ifndef CSRC_DECODER_SPEEX_H
#define CSRC_DECODER_SPEEX_H

#include "../decoder.h"
#include <ttLibC/frame/audio/pcmS16.h>
#ifdef __ENABLE_SPEEX__
# include <ttLibC/decoder/speexDecoder.h>
#endif

class SpeexDecoder : public Decoder {
public:
  SpeexDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_PcmS16 *pcm);
  ~SpeexDecoder();
#ifdef __ENABLE_SPEEX__
  ttLibC_SpeexDecoder *decoder_;
#endif
};

#endif
