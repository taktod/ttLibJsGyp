#ifndef CSRC_ENCODER_THEORA_H
#define CSRC_ENCODER_THEORA_H

#include "../encoder.h"
#include <ttLibC/frame/video/theora.h>
#ifdef __ENABLE_THEORA__
# include <ttLibC/encoder/theoraEncoder.h>
#endif

class TheoraEncoder : public Encoder {
public:
  TheoraEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Theora *frame);
  ~TheoraEncoder();
#ifdef __ENABLE_THEORA__
  ttLibC_TheoraEncoder *encoder_;
#endif
};

#endif