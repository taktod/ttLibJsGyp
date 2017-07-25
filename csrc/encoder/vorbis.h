#ifndef CSRC_ENCODER_VORBIS_H
#define CSRC_ENCODER_VORBIS_H

#include "../encoder.h"
#include <ttLibC/frame/audio/vorbis.h>
#ifdef __ENABLE_VORBIS_ENCODE__
# include <ttLibC/encoder/vorbisEncoder.h>
#endif

class VorbisEncoder : public Encoder {
public:
  VorbisEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Vorbis *frame);
  ~VorbisEncoder();
#ifdef __ENABLE_VORBIS_ENCODE__
  ttLibC_VorbisEncoder *encoder_;
#endif
};

#endif