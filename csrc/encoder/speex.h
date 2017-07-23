#ifndef CSRC_ENCODER_SPEEX_H
#define CSRC_ENCODER_SPEEX_H

#include "../encoder.h"
#include <ttLibC/frame/audio/speex.h>
#ifdef __ENABLE_SPEEX__
# include <ttLibC/encoder/speexEncoder.h>
#endif

class SpeexEncoder : public Encoder {
public:
  SpeexEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Speex *frame);
  ~SpeexEncoder();
#ifdef __ENABLE_SPEEX__
  ttLibC_SpeexEncoder *encoder_;
#endif
};

#endif