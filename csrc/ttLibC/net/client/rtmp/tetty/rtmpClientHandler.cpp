﻿#include "rtmpClientHandler.h"

#include <ttLibC/net/client/rtmp/message/rtmpMessage.h>
#include <ttLibC/net/client/rtmp/message/aggregateMessage.h>
#include <ttLibC/net/client/rtmp/message/setChunkSize.h>
#include <ttLibC/net/client/rtmp/message/audioMessage.h>
#include <ttLibC/net/client/rtmp/message/videoMessage.h>
#include <ttLibC/net/client/rtmp/message/setPeerBandwidth.h>
#include <ttLibC/net/client/rtmp/message/userControlMessage.h>
#include <ttLibC/net/client/rtmp/message/acknowledgement.h>
#include <ttLibC/net/client/rtmp/message/windowAcknowledgementSize.h>
#include <ttLibC/allocator.h>
#include "log.h"

typedef struct frameCallbackData_t {
  RtmpBootstrap *bootstrap;
  uint32_t streamId;
} frameCallbackData_t;

static bool RtmpClientHander_frameCallback(void *ptr, ttLibC_Frame *frame) {
  frameCallbackData_t *data = (frameCallbackData_t *)ptr;
  return data->bootstrap->playFrameCallback(data->streamId, frame);
}

static tetty_errornum RtmpClientHandler_channelRead(
    ttLibC_TettyContext *ctx,
    void *data,
    size_t data_size) {
  (void)data_size;
  ttLibC_RtmpMessage *rtmp_message = (ttLibC_RtmpMessage *)data;
  ttLibC_ClientObject *client_object = (ttLibC_ClientObject *)ctx->socket_info->ptr;
  ttLibC_RtmpClientHandler *handler = (ttLibC_RtmpClientHandler *)ctx->channel_handler;
  RtmpBootstrap *bootstrap = ((ttLibC_RtmpClientHandler2 *)handler)->bootstrap;
  // update bytesRead
  handler->bytesRead += rtmp_message->header->size;
  // ackの送信はnodeの動作の場合ここでやる(updateを呼ばないため、あっちではできない)
  if(handler->bytesRead - handler->bytesReadAcked >= handler->bytesReadWindow) {
    // send ack.
    ttLibC_Acknowledgement *acknowledgement = ttLibC_Acknowledgement_make((uint32_t)handler->bytesRead);
    ttLibC_TettyContext_channel_write(ctx, acknowledgement, sizeof(ttLibC_Acknowledgement));
    ttLibC_TettyContext_channel_flush(ctx);
    ttLibC_Acknowledgement_close(&acknowledgement);
    handler->bytesReadAcked = handler->bytesRead;
  }
  switch(rtmp_message->header->message_type) {
  case RtmpMessageType_videoMessage:
    {
      frameCallbackData_t data;
      data.bootstrap = bootstrap;
      data.streamId  = rtmp_message->header->stream_id;
      ttLibC_FlvFrameManager *frameManager = bootstrap->refFrameManager(rtmp_message->header->stream_id);
      tetty_errornum error_num = ttLibC_VideoMessage_getFrame(
        (ttLibC_VideoMessage *)rtmp_message,
        frameManager,
        RtmpClientHander_frameCallback,
        &data);
      if(error_num != 0) {
        LOG_PRINT("something happen during videoMessage_getFrame.:%d", error_num);
        ctx->bootstrap->error_number = error_num;
      }
    }
    break;
  case RtmpMessageType_audioMessage:
    {
      frameCallbackData_t data;
      data.bootstrap = bootstrap;
      data.streamId  = rtmp_message->header->stream_id;
      ttLibC_FlvFrameManager *frameManager = bootstrap->refFrameManager(rtmp_message->header->stream_id);
      tetty_errornum error_num = ttLibC_AudioMessage_getFrame(
        (ttLibC_AudioMessage *)rtmp_message,
        frameManager,
        RtmpClientHander_frameCallback,
        &data);
      if(error_num != 0) {
        LOG_PRINT("something happen during audioMessage_getFrame.:%d", error_num);
        ctx->bootstrap->error_number = error_num;
      }
    }
    break;
  case RtmpMessageType_aggregateMessage:
    {
      frameCallbackData_t data;
      data.bootstrap = bootstrap;
      data.streamId  = rtmp_message->header->stream_id;
      ttLibC_FlvFrameManager *frameManager = bootstrap->refFrameManager(rtmp_message->header->stream_id);
      tetty_errornum error_num = ttLibC_AggregateMessage_getFrame(
        (ttLibC_AggregateMessage *)rtmp_message,
        frameManager,
        RtmpClientHander_frameCallback,
        &data);
      if(error_num != 0) {
        LOG_PRINT("something happen during aggregateMessage_getFrame.:%d", error_num);
        ctx->bootstrap->error_number = error_num;
      }
    }
    break;
  case RtmpMessageType_setChunkSize:
    {
      ttLibC_SetChunkSize *chunk_size = (ttLibC_SetChunkSize *)rtmp_message;
      client_object->recv_chunk_size = chunk_size->size;
    }
    return 0;
  case RtmpMessageType_windowAcknowledgementSize:
    {
      // for win ack, send it back to server.
      ttLibC_WindowAcknowledgementSize *win_ack = (ttLibC_WindowAcknowledgementSize *)rtmp_message;
      ttLibC_TettyContext_channel_write(ctx, win_ack, sizeof(ttLibC_WindowAcknowledgementSize));
      ttLibC_TettyContext_channel_flush(ctx);
      ttLibC_WindowAcknowledgementSize *windowAcknowledgementSize = (ttLibC_WindowAcknowledgementSize *)rtmp_message;
      handler->bytesReadWindow = windowAcknowledgementSize->size;
    }
    break;
  case RtmpMessageType_setPeerBandwidth:
    {
      ttLibC_SetPeerBandwidth *setPeerBandwidth = (ttLibC_SetPeerBandwidth *)rtmp_message;
      handler->bytesWrittenWindow = setPeerBandwidth->size;
    }
    break;
    // need to add more.(like userControlMessage for ping/pong.)
  case RtmpMessageType_userControlMessage:
    {
      ttLibC_UserControlMessage *user_control_message = (ttLibC_UserControlMessage *)rtmp_message;
      switch(user_control_message->type) {
      case Type_StreamBegin:
        // nothing to do. leave.
        break;
      case Type_StreamEof:
        // nothing to do. leave.
        break;
/*      case Type_StreamDry:
      case Type_RecordedStream:*/
      case Type_BufferEmpty:
        break;
      case Type_BufferFull:
        break;
/*      case Type_ClientBufferLength:
      case Type_Unknown5:*/
      case Type_Ping:
        {
          // reply pong.
          ttLibC_UserControlMessage *pong = ttLibC_UserControlMessage_pong(user_control_message->time);
          ttLibC_TettyContext_channel_write(ctx, pong, sizeof(ttLibC_UserControlMessage));
          ttLibC_TettyContext_channel_flush(ctx);
          ttLibC_UserControlMessage_close(&pong);
        }
        break;
/*      case Type_Pong:
      case Type_Unknown8:
      case Type_PingSwfVerification:
      case Type_PongSwfVerification:*/
      default:
        ERR_PRINT("unknown userControlMessage:%d", user_control_message->type);
        return 0;
      }
    }
    break;
  default:
    break;
  }
  return 0;
}

static tetty_errornum RtmpClientHandler_write(
    ttLibC_TettyContext *ctx,
    void *data,
    size_t data_size) {
  if(ctx->socket_info == NULL) {
    return ttLibC_TettyContext_super_writeEach(ctx, data, data_size);
  }
  else {
    return ttLibC_TettyContext_super_write(ctx, data, data_size);
  }
}

ttLibC_RtmpClientHandler *ttLibC_RtmpClientHandler_make() {
  ttLibC_RtmpClientHandler *handler = (ttLibC_RtmpClientHandler *)ttLibC_malloc(sizeof(ttLibC_RtmpClientHandler2));
  if(handler == NULL) {
    return NULL;
  }
  memset(handler, 0, sizeof(ttLibC_RtmpClientHandler2));
  handler->bytesReadWindow = 2500000;
  handler->bytesWrittenWindow = 2500000;
  handler->channel_handler.channelRead = RtmpClientHandler_channelRead;
  handler->channel_handler.write = RtmpClientHandler_write;
  return handler;
}

ttLibC_RtmpClientHandler *ttLibC_RtmpClientHandler2_make(RtmpBootstrap *bootstrap) {
  ttLibC_RtmpClientHandler2 *handler = (ttLibC_RtmpClientHandler2 *)ttLibC_RtmpClientHandler_make();
  handler->bootstrap = bootstrap;
  return (ttLibC_RtmpClientHandler *)handler;
}

void ttLibC_RtmpClientHandler_close(ttLibC_RtmpClientHandler **handler) {
  ttLibC_RtmpClientHandler *target = (ttLibC_RtmpClientHandler *)*handler;
  if(target == NULL) {
    return;
  }
  ttLibC_free(target);
  *handler = NULL;
}
