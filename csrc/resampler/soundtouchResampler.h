#ifndef CSRC_RESAMPLER_SOUNDTOUCH_H
#define CSRC_RESAMPLER_SOUNDTOUCH_H

#include "../resampler.h"
#include <ttLibC/frame/audio/audio.h>
#ifdef __ENABLE_SOUNDTOUCH__
# include <ttLibC/resampler/soundtouchResampler.h>
#endif

class SoundtouchResampler : public Resampler {
public:
  SoundtouchResampler(Local<Object> params);
  bool resample(ttLibC_Frame *frame);
  void setRate(double newRate);
  void setTempo(double newTempo);
  void setRateChange(double newRate);
  void setTempoChange(double newTempo);
  void setPitch(double newPitch);
  void setPitchOctaves(double newPitch);
  void setPitchSemiTones(double newPitch);
private:
  static bool resampleCallback(void *ptr, ttLibC_Audio *frame);
  ~SoundtouchResampler();
#ifdef __ENABLE_SOUNDTOUCH__
  ttLibC_Soundtouch *resampler_;
#endif
};

#endif