#ifndef CSRC_TTLIBC_NET_CLIENT_RTMP_TETTY_RTMPCLIENTHANDLER_H
#define CSRC_TTLIBC_NET_CLIENT_RTMP_TETTY_RTMPCLIENTHANDLER_H

#include <ttLibC/net/client/rtmp.h>
#include <ttLibC/net/client/rtmp/tetty/rtmpClientHandler.h>
#include "../rtmpBootstrap.h"

typedef struct ttLibC_Net_Client_Rtmp_Tetty_RtmpClientHandler2 {
  ttLibC_RtmpClientHandler inherit_super;
  RtmpBootstrap *bootstrap;
} ttLibC_Net_Client_Rtmp_Tetty_RtmpClientHandler2;
typedef ttLibC_Net_Client_Rtmp_Tetty_RtmpClientHandler2 ttLibC_RtmpClientHandler2;

ttLibC_RtmpClientHandler *ttLibC_RtmpClientHandler2_make(RtmpBootstrap *bootstrap);

#endif