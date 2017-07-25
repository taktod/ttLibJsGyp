#ifndef CSRC_RESAMPLER_IMAGE_H
#define CSRC_RESAMPLER_IMAGE_H

#include "../resampler.h"
#include <ttLibC/frame/video/video.h>

class ImageResampler : public Resampler {
public:
  ImageResampler(Local<Object> params);
  bool resample(ttLibC_Frame *frame);
private:
  ~ImageResampler();
  ttLibC_Frame_Type frameType_;
  uint32_t          subType_;
  ttLibC_Video     *prevFrame_;
};

#endif