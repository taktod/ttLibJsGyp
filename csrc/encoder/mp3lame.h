#ifndef CSRC_ENCODER_MP3LAME_H
#define CSRC_ENCODER_MP3LAME_H

#include "../encoder.h"
#include <ttLibC/frame/audio/mp3.h>
#ifdef __ENABLE_MP3LAME_ENCODE__
# include <ttLibC/encoder/mp3lameEncoder.h>
#endif

class Mp3lameEncoder : public Encoder {
public:
  Mp3lameEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Mp3 *frame);
  ~Mp3lameEncoder();
#ifdef __ENABLE_MP3LAME_ENCODE__
  ttLibC_Mp3lameEncoder *encoder_;
#endif
};

#endif
