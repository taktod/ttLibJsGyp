#ifndef CSRC_ENCODER_MSAAC_H
#define CSRC_ENCODER_MSAAC_H

#include "../encoder.h"
#include <ttLibC/frame/audio/aac.h>
#ifdef __ENABLE_WIN32__
# include <ttLibC/encoder/msAacEncoder.h>
#endif

class MSAacEncoder : public Encoder {
public:
  MSAacEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Aac *aac);
  ~MSAacEncoder();
#ifdef __ENABLE_WIN32__
  ttLibC_MsAacEncoder *encoder_;
#endif
};

#endif
