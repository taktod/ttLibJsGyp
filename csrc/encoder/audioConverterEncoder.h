#ifndef CSRC_ENCODER_AUDIOCONVERTER_H
#define CSRC_ENCODER_AUDIOCONVERTER_H

#include "../encoder.h"
#include <ttLibC/frame/audio/audio.h>
#ifdef __ENABLE_APPLE__
# include <ttLibC/encoder/audioConverterEncoder.h>
#endif

class AudioConverterEncoder : public Encoder {
public:
  AudioConverterEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Audio *frame);
  ~AudioConverterEncoder();
#ifdef __ENABLE_APPLE__
  ttLibC_AcEncoder *encoder_;
#endif
};

#endif