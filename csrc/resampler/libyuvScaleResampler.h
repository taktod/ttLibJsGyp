#ifndef CSRC_RESAMPLE_LIBYUVSCALE_H
#define CSRC_RESAMPLE_LIBYUVSCALE_H

#include "../resampler.h"
#include <ttLibC/frame/video/yuv420.h>
#ifdef __ENABLE_LIBYUV__
# include <ttLibC/resampler/libyuvResampler.h>
#endif

class LibyuvScaleResampler : public Resampler {
public:
  LibyuvScaleResampler(Local<Object> params);
  bool resample(ttLibC_Frame *frame);
private:
  ~LibyuvScaleResampler();
#ifdef __ENABLE_LIBYUV__
  ttLibC_LibyuvFilter_Mode yMode_;
  ttLibC_LibyuvFilter_Mode uMode_;
  ttLibC_LibyuvFilter_Mode vMode_;
  uint32_t width_;
  uint32_t height_;
#endif
  ttLibC_Yuv420 *prevFrame_;
};

#endif