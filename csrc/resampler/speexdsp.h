#ifndef CSRC_RESAMPLER_SPEEXDSP_H
#define CSRC_RESAMPLER_SPEEXDSP_H

#include "../resampler.h"
#include <ttLibC/frame/audio/pcmS16.h>
#ifdef __ENABLE_SPEEXDSP__
# include <ttLibC/resampler/speexdspResampler.h>
#endif

class SpeexdspResampler : public Resampler {
public:
  SpeexdspResampler(Local<Object> params);
  bool resample(ttLibC_Frame *frame);
private:
  ~SpeexdspResampler();
#ifdef __ENABLE_SPEEXDSP__
  ttLibC_SpeexdspResampler *resampler_;
#endif
  ttLibC_PcmS16 *pcm_;
};

#endif
