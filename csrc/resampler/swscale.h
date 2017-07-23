#ifndef CSRC_RESAMPLE_SWSCALE_H
#define CSRC_RESAMPLE_SWSCALE_H

#include "../resampler.h"
#include <ttLibC/frame/video/bgr.h>
#include <ttLibC/frame/video/yuv420.h>
#ifdef __ENABLE_SWSCALE__
# include <ttLibC/resampler/swscaleResampler.h>
#endif

class SwscaleResampler : public Resampler {
public:
  SwscaleResampler(Local<Object> params);
  bool resample(ttLibC_Frame *frame);
private:
  static bool resampleCallback(void *ptr, ttLibC_Frame *frame);
  uint32_t getSubType(ttLibC_Frame_Type type, std::string subType);
  ~SwscaleResampler();
#ifdef __ENABLE_SWSCALE__
  ttLibC_SwscaleResampler *resampler_;
#endif
};

#endif