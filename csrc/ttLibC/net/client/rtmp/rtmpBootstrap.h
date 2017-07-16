#ifndef CSRC_TTLIBC_NET_CLIENT_RTMP_RTMPBOOTSTRAP_H
#define CSRC_TTLIBC_NET_CLIENT_RTMP_RTMPBOOTSTRAP_H

#include "../../tetty/bootstrap.h"
#include <ttLibC/net/client/rtmp/message/amf0Command.h>
#include <ttLibC/util/stlMapUtil.h>
#include <ttLibC/util/flvFrameUtil.h>
#include <ttLibC/container/misc.h>

typedef struct ttg_frameGroup {
  ttLibC_Frame_Type audioType;
  ttLibC_FrameQueue *audioQueue;
  ttLibC_Frame_Type videoType;
  ttLibC_FrameQueue *videoQueue;
} ttg_frameGroup;

class RtmpBootstrap : public Bootstrap {
public:
  static void classInit(Local<Object> target);

  ttLibC_FlvFrameManager *refFrameManager(uint32_t streamId);

  void recvAmf0Command(ttLibC_Amf0Command *command);
  void recvAmf0Result(ttLibC_Amf0Command *command, ttLibC_Amf0Command *result);
  bool playFrameCallback(uint32_t streamId, ttLibC_Frame *frame);

  void sendConnect();
private:
  static NAN_METHOD(New);
  static NAN_METHOD(Connect);
  static NAN_METHOD(Data);

  static NAN_METHOD(CreateStream);
  static NAN_METHOD(Play);
  static NAN_METHOD(Publish);
  static NAN_METHOD(SetBufferLength);
  static NAN_METHOD(QueueFrame);
  static NAN_METHOD(CloseStream);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  Local<Value> toJsObject(ttLibC_Amf0Object *src_obj);
  static bool frameManagerCloseCallback(void *ptr, void *key, void *item);
  static bool frameGroupCloseCallback(void *ptr, void *key, void *item);
  RtmpBootstrap(Local<Value> address, Local<Value> app);
  ~RtmpBootstrap();

  std::string address_;
  std::string app_;

  Nan::Persistent<Object> socket_;

  ttLibC_TettyChannelHandler *handshake_;
  ttLibC_TettyChannelHandler *decoder_;
  ttLibC_TettyChannelHandler *encoder_;
  ttLibC_TettyChannelHandler *commandHandler_;
  ttLibC_TettyChannelHandler *clientHandler_;

  // streamId -> frameManagerのmap playでのみ利用する。
  ttLibC_StlMap *streamIdFlvFrameManagerMap_;
  // streamId -> ttg_frameGroup
  ttLibC_StlMap *frameGroupMap_;
};

#endif
