#ifndef CSRC_RESAMPLER_AUDIO_H
#define CSRC_RESAMPLER_AUDIO_H

#include "../resampler.h"
#include <ttLibC/frame/audio/audio.h>

class AudioResampler : public Resampler {
public:
  AudioResampler(Local<Object> params);
  bool resample(ttLibC_Frame *frame);
private:
  ~AudioResampler();
  ttLibC_Frame_Type frameType_;
  uint32_t          subType_;
  uint32_t          channelNum_;
  ttLibC_Audio     *prevFrame_;
};

#endif
