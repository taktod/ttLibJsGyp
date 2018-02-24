#ifndef CSRC_RESAMPLE_LIBYUVROTATE_H
#define CSRC_RESAMPLE_LIBYUVROTATE_H

#include "../resampler.h"
#include <ttLibC/frame/video/yuv420.h>
#ifdef __ENABLE_LIBYUV__
# include <ttLibC/resampler/libyuvResampler.h>
#endif

class LibyuvRotateResampler : public Resampler {
public:
  LibyuvRotateResampler(Local<Object> params);
  bool resample(ttLibC_Frame *frame);
private:
  ~LibyuvRotateResampler();
#ifdef __ENABLE_LIBYUV__
  ttLibC_LibyuvRotate_Mode mode_;
#endif
  ttLibC_Yuv420 *prevFrame_;
};

#endif