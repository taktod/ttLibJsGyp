#ifndef CSRC_DECODER_VTDECOMPRESSSESSION_H
#define CSRC_DECODER_VTDECOMPRESSSESSION_H

#include "../decoder.h"
#include <ttLibC/frame/video/yuv420.h>
#ifdef __ENABLE_OPENH264__
# include <ttLibC/decoder/vtDecompressSessionDecoder.h>
# include <pthread.h>
#endif
#include <ttLibC/util/stlListUtil.h>

class VtDecompressSessionDecoder : public Decoder {
public:
  VtDecompressSessionDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_Yuv420 *yuv);
  ~VtDecompressSessionDecoder();
#ifdef __ENABLE_OPENH264__
  ttLibC_VtDecoder *decoder_;
  pthread_mutex_t frameMutex_;
#endif
  ttLibC_StlList *frameStack_;
};

#endif
