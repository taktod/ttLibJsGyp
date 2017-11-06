#ifndef CSRC_RESAMPLE_SWRESAMPLE_H
#define CSRC_RESAMPLE_SWRESAMPLE_H

#include "../resampler.h"
#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/frame/audio/pcmf32.h>
#ifdef __ENABLE_SWRESAMPLE__
# include <ttLibC/resampler/swresampleResampler.h>
#endif

class SwresampleResampler : public Resampler {
public:
  SwresampleResampler(Local<Object> params);
  bool resample(ttLibC_Frame *frame);
private:
  static bool resampleCallback(void *ptr, ttLibC_Frame *frame);
  uint32_t getSubType(ttLibC_Frame_Type type, std::string subType);
  ~SwresampleResampler();
#ifdef __ENABLE_SWRESAMPLE__
  ttLibC_SwresampleResampler *resampler_;
#endif
};

#endif
