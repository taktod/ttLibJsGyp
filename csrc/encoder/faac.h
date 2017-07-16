#ifndef CSRC_ENCODER_FAAC_H
#define CSRC_ENCODER_FAAC_H

#include "../encoder.h"
#include <ttLibC/frame/audio/aac.h>
#ifdef __ENABLE_FAAC_ENCODE__
# include <ttLibC/encoder/faacEncoder.h>
#endif

class FaacEncoder : public Encoder {
public:
  FaacEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Aac *frame);
  ~FaacEncoder();
#ifdef __ENABLE_FAAC_ENCODE__
  ttLibC_FaacEncoder *encoder_;
#endif
};

#endif