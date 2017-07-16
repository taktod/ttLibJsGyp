#include <nan.h>

using namespace v8;

#include "reader.h"
#include "writer.h"
#include "decoder.h"
#include "encoder.h"
#include "resampler.h"
#include "ttLibC/net/client/rtmp/rtmpBootstrap.h"
#include "msLoopback.h"
#include "frame.h"
#include "msSetup.h"

static NAN_MODULE_INIT(Init) {
  Reader::classInit(target);
  Writer::classInit(target);
  Decoder::classInit(target);
  Encoder::classInit(target);
  Resampler::classInit(target);
  MSLoopback::classInit(target);
  Frame::classInit(target);
  RtmpBootstrap::classInit(target);
  MsSetupInit(target);
}

NODE_MODULE(ttLibJsGyp, Init);
