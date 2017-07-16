#ifndef CSRC_TTLIBC_NET_CLIENT_RTMP_TETTY_RTMPCOMMANDHANDLER_H
#define CSRC_TTLIBC_NET_CLIENT_RTMP_TETTY_RTMPCOMMANDHANDLER_H

#include <ttLibC/net/client/rtmp.h>
#include <ttLibC/net/client/rtmp/tetty/rtmpCommandHandler.h>
#include "../rtmpBootstrap.h"

typedef struct ttLibC_Net_Client_Rtmp_Tetty_RtmpCommandHandler2 {
  ttLibC_RtmpCommandHandler inherit_super;
  RtmpBootstrap *bootstrap;
} ttLibC_Net_Client_Rtmp_Tetty_RtmpCommandHandler2;
typedef ttLibC_Net_Client_Rtmp_Tetty_RtmpCommandHandler2 ttLibC_RtmpCommandHandler2;

ttLibC_RtmpCommandHandler *ttLibC_RtmpCommandHandler2_make(RtmpBootstrap *bootstrap);

#endif