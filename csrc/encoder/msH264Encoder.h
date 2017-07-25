#ifndef CSRC_ENCODER_MSH264_H
#define CSRC_ENCODER_MSH264_H

#include "../encoder.h"
#include <ttLibC/frame/video/h264.h>
#ifdef __ENABLE_WIN32__
# include <windows.h>
# include <ttLibC/encoder/msH264Encoder.h>
#endif
#include <ttLibC/util/stlListUtil.h>

class MSH264Encoder : public Encoder {
public:
  static void classInit(Local<Object> target);
  MSH264Encoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  static bool listEncoderCallback(void *ptr, const char *name);
  static NAN_METHOD(ListEncoders);
  static bool encodeCallback(void *ptr, ttLibC_H264 *h264);
  ~MSH264Encoder();
#ifdef __ENABLE_WIN32__
  ttLibC_MsH264Encoder *encoder_;
  HANDLE hMutex_;
  ttLibC_StlList *frameStack_;
#endif
};

#endif
