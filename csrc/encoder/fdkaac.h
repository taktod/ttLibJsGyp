#ifndef CSRC_ENCODER_FDKAAC_H
#define CSRC_ENCODER_FDKAAC_H

#include "../encoder.h"
#include <ttLibC/frame/audio/aac.h>
#include <ttLibC/frame/audio/pcms16.h>
#ifdef __ENABLE_FDKAAC_ENCODE__
# include <fdk-aac/aacenc_lib.h>
#endif

class FdkaacEncoder : public Encoder {
public:
  FdkaacEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
private:
  bool encodeCallback(ttLibC_Aac *frame);
  ~FdkaacEncoder();
#ifdef __ENABLE_FDKAAC_ENCODE__
  HANDLE_AACENCODER handle_;
  ttLibC_Aac *aac_;
	uint8_t    *pcm_buffer_;
	size_t      pcm_buffer_size_;
	size_t      pcm_buffer_next_pos_;
	uint8_t    *data_;
	size_t      data_size_;
	uint64_t    pts_;
	uint32_t    sample_rate_;
	uint32_t    sample_num_;
	bool        is_pts_initialized_;
#endif
};

#endif