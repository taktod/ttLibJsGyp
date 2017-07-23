#ifndef CSRC_DECODER_VORBIS_H
#define CSRC_DECODER_VORBIS_H

#include "../decoder.h"
#include <ttLibC/frame/audio/pcmF32.h>
#ifdef __ENABLE_VORBIS_DECODE__
# include <ttLibC/decoder/vorbisDecoder.h>
#endif

class VorbisDecoder : public Decoder {
public:
  VorbisDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_PcmF32 *pcm);
  ~VorbisDecoder();
#ifdef __ENABLE_VORBIS_DECODE__
  ttLibC_VorbisDecoder *decoder_;
#endif
};

#endif
