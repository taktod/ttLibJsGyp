#ifndef CSRC_ENCODER_VTCOMPRESSSESSION_H
#define CSRC_ENCODER_VTCOMPRESSSESSION_H

#include "../encoder.h"
#include <ttLibC/frame/video/video.h>
#ifdef __ENABLE_APPLE__
# include <ttLibC/encoder/vtCompressSessionEncoder.h>
# include <pthread.h>
#endif
#include <ttLibC/util/stlListUtil.h>

class VtCompressSessionEncoder : public Encoder {
public:
  VtCompressSessionEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Video *frame);
  ~VtCompressSessionEncoder();
#ifdef __ENABLE_APPLE__
  ttLibC_VtEncoder *encoder_;
  pthread_mutex_t frameMutex_;
#endif
  ttLibC_StlList *frameStack_;
};

#endif