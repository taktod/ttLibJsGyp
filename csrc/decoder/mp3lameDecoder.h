#ifndef CSRC_DECODER_MP3LAME_H
#define CSRC_DECODER_MP3LAME_H

#include "../decoder.h"
#include <ttLibC/frame/audio/pcms16.h>
#ifdef __ENABLE_MP3LAME_DECODE__
# include <ttLibC/decoder/mp3lameDecoder.h>
#endif

class Mp3lameDecoder : public Decoder {
public:
  Mp3lameDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_PcmS16 *pcm);
  ~Mp3lameDecoder();
#ifdef __ENABLE_MP3LAME_DECODE__
  ttLibC_Mp3lameDecoder *decoder_;
#endif
};

#endif
