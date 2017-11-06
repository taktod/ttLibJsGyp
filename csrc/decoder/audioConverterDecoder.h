#ifndef CSRC_DECODER_AUDIOCONVERTER_H
#define CSRC_DECODER_AUDIOCONVERTER_H

#include "../decoder.h"
#include <ttLibC/frame/audio/pcms16.h>
#ifdef __ENABLE_APPLE__
# include <ttLibC/decoder/audioConverterDecoder.h>
#endif

class AudioConverterDecoder : public Decoder {
public:
  AudioConverterDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_PcmS16 *pcm);
  ~AudioConverterDecoder();
#ifdef __ENABLE_APPLE__
  ttLibC_AcDecoder *decoder_;
#endif
};

#endif
