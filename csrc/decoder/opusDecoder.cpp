#include "opusDecoder.h"
#include "../frame.h"
#include <opus/opus.h>

OpusDecoder_::OpusDecoder_(Local<Object> params) : Decoder() {
  type_ = gdt_opus;
#ifdef __ENABLE_OPUS__
  uint32_t sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  decoder_ = ttLibC_OpusDecoder_make(sampleRate, channelNum);
#endif
}

OpusDecoder_::~OpusDecoder_() {
#ifdef __ENABLE_OPUS__
  ttLibC_OpusDecoder_close(&decoder_);
#endif
}

bool OpusDecoder_::decodeCallback(void *ptr, ttLibC_PcmS16 *pcm) {
  OpusDecoder_ *decoder = (OpusDecoder_ *)ptr;
  Nan::Callback callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)pcm);
  Local<Value> args[] = {
    jsFrame
  };
  Local<Value> result = callback.Call(1, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

bool OpusDecoder_::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_OPUS__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_opus) {
    puts("opusのみ処理可能です。");
    return false;
  }
  return ttLibC_OpusDecoder_decode(
    decoder_,
    (ttLibC_Opus *)frame,
    decodeCallback,
    this);
#else
  return false;
#endif
}

int OpusDecoder_::codecControl(std::string control, int value) {
  if(decoder_ == NULL) {
    puts("decoderが準備されていません。");
    return -1;
  }
  OpusDecoder *nativeDecoder = (OpusDecoder *)ttLibC_OpusDecoder_refNativeDecoder(decoder_);
#define OpusSetCtl(a, b) if(control == #a) { \
    return opus_decoder_ctl(nativeDecoder, a(b));\
  }
#define OpusCtl(a) if(control == #a) { \
    return opus_decoder_ctl(nativeDecoder, a);\
  }
#define OpusGetCtl(a) if(control == #a) { \
    int result, res; \
    res = opus_decoder_ctl(nativeDecoder, a(&result)); \
    if(res != OPUS_OK) {return res;} \
    return result; \
  }
#define OpusGetUCtl(a) if(control == #a) { \
    uint32_t result; \
    int res; \
    res = opus_decoder_ctl(nativeDecoder, a(&result)); \
    if(res != OPUS_OK) {return res;} \
    return result; \
  }
  OpusCtl(OPUS_RESET_STATE)
  OpusGetUCtl(OPUS_GET_FINAL_RANGE)
  OpusGetCtl(OPUS_GET_BANDWIDTH)
  OpusGetCtl(OPUS_GET_SAMPLE_RATE)
  OpusSetCtl(OPUS_SET_PHASE_INVERSION_DISABLED, value)
  OpusGetCtl(OPUS_GET_PHASE_INVERSION_DISABLED)
  OpusSetCtl(OPUS_SET_GAIN, value)
  OpusGetCtl(OPUS_GET_GAIN)
  OpusGetCtl(OPUS_GET_LAST_PACKET_DURATION)
  OpusGetCtl(OPUS_GET_PITCH)
#undef OpusSetCtl
#undef OpusCtl
#undef OpusGetCtl
#undef OpusGetUCtl
  return OPUS_OK;
}
