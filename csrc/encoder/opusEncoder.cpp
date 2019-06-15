#include "opusEncoder.h"
#include "../frame.h"
#include "../util.h"
#include <opus/opus.h>

OpusEncoder_::OpusEncoder_(Local<Object> params) : Encoder() {
  type_ = get_opus;
#ifdef __ENABLE_OPUS__
  uint32_t sampleRate    = Uint32Value(Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked());
  uint32_t channelNum    = Uint32Value(Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked());
  uint32_t unitSampleNum = Uint32Value(Nan::Get(params, Nan::New("unitSampleNum").ToLocalChecked()).ToLocalChecked());
  encoder_ = ttLibC_OpusEncoder_make(sampleRate, channelNum, unitSampleNum);
#endif
}

OpusEncoder_::~OpusEncoder_() {
#ifdef __ENABLE_OPUS__
  ttLibC_OpusEncoder_close(&encoder_);
#endif
}

bool OpusEncoder_::encodeCallback(void *ptr, ttLibC_Opus *opus) {
  OpusEncoder_ *encoder = (OpusEncoder_ *)ptr;
  Nan::Callback callback(encoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(encoder->jsFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)opus);
  Local<Value> args[] = {
    jsFrame
  };
  Local<Value> result = callbackCall(callback, 1, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

bool OpusEncoder_::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_OPUS__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != frameType_pcmS16) {
    puts("pcmS16のみ処理可能です。");
    return false;
  }
  return ttLibC_OpusEncoder_encode(
    encoder_,
    (ttLibC_PcmS16 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}

bool OpusEncoder_::setBitrate(uint32_t value) {
#ifdef __ENABLE_OPUS__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  return ttLibC_OpusEncoder_setBitrate(encoder_, value);
#else
  return false;
#endif
}

bool OpusEncoder_::setComplexity(uint32_t value) {
#ifdef __ENABLE_OPUS__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  return ttLibC_OpusEncoder_setComplexity(encoder_, value);
#else
  return false;
#endif
}

int OpusEncoder_::codecControl(std::string control, int value) {
#ifdef __ENABLE_OPUS__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return -1;
  }
  OpusEncoder *nativeEncoder = (OpusEncoder *)ttLibC_OpusEncoder_refNativeEncoder(encoder_);
#define OpusSetCtl(a, b) if(control == #a) { \
    return opus_encoder_ctl(nativeEncoder, a(b));\
  }
#define OpusCtl(a) if(control == #a) { \
    return opus_encoder_ctl(nativeEncoder, a);\
  }
#define OpusGetCtl(a) if(control == #a) { \
    int result, res; \
    res = opus_encoder_ctl(nativeEncoder, a(&result)); \
    if(res != OPUS_OK) {return res;} \
    return result; \
  }
#define OpusGetUCtl(a) if(control == #a) { \
    uint32_t result; \
    int res; \
    res = opus_encoder_ctl(nativeEncoder, a(&result)); \
    if(res != OPUS_OK) {return res;} \
    return result; \
  }
  OpusSetCtl(OPUS_SET_COMPLEXITY, value)
  OpusGetCtl(OPUS_GET_COMPLEXITY)
  OpusSetCtl(OPUS_SET_BITRATE, value)
  OpusGetCtl(OPUS_GET_BITRATE)
  OpusSetCtl(OPUS_SET_VBR, value)
  OpusGetCtl(OPUS_GET_VBR)
  OpusSetCtl(OPUS_SET_VBR_CONSTRAINT, value)
  OpusGetCtl(OPUS_GET_VBR_CONSTRAINT)
  OpusSetCtl(OPUS_SET_FORCE_CHANNELS, value)
  OpusGetCtl(OPUS_GET_FORCE_CHANNELS)
  OpusSetCtl(OPUS_SET_MAX_BANDWIDTH, value)
  OpusGetCtl(OPUS_GET_MAX_BANDWIDTH)
  OpusSetCtl(OPUS_SET_BANDWIDTH, value)
  OpusSetCtl(OPUS_SET_SIGNAL, value)
  OpusGetCtl(OPUS_GET_SIGNAL)
  OpusSetCtl(OPUS_SET_APPLICATION, value)
  OpusGetCtl(OPUS_GET_APPLICATION)
  OpusGetCtl(OPUS_GET_LOOKAHEAD)
  OpusSetCtl(OPUS_SET_INBAND_FEC, value)
  OpusGetCtl(OPUS_GET_INBAND_FEC)
  OpusSetCtl(OPUS_SET_PACKET_LOSS_PERC, value)
  OpusGetCtl(OPUS_GET_PACKET_LOSS_PERC)
  OpusSetCtl(OPUS_SET_DTX, value)
  OpusGetCtl(OPUS_GET_DTX)
  OpusSetCtl(OPUS_SET_LSB_DEPTH, value)
  OpusGetCtl(OPUS_GET_LSB_DEPTH)
  OpusSetCtl(OPUS_SET_EXPERT_FRAME_DURATION, value)
  OpusGetCtl(OPUS_GET_EXPERT_FRAME_DURATION)
  OpusSetCtl(OPUS_SET_PREDICTION_DISABLED, value)
  OpusGetCtl(OPUS_GET_PREDICTION_DISABLED)
  OpusCtl(OPUS_RESET_STATE)
  OpusGetUCtl(OPUS_GET_FINAL_RANGE)
  OpusGetCtl(OPUS_GET_BANDWIDTH)
  OpusGetCtl(OPUS_GET_SAMPLE_RATE)
  OpusSetCtl(OPUS_SET_PHASE_INVERSION_DISABLED, value)
  OpusGetCtl(OPUS_GET_PHASE_INVERSION_DISABLED)
#undef OpusSetCtl
#undef OpusCtl
#undef OpusGetCtl
#undef OpusGetUCtl
  return OPUS_OK;
#else
  return -1;
#endif
}