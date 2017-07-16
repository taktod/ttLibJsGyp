// コマンド系の処理用
#include "rtmpCommandHandler.h"

#include <ttLibC/allocator.h>
#include <ttLibC/net/client/rtmp/message/rtmpMessage.h>
#include <ttLibC/net/client/rtmp/message/amf0Command.h>
#include "log.h"
#include <string>

static tetty_errornum RtmpCommandHandler_channelActive(
    ttLibC_TettyContext *ctx) {
  // channelActiveになったということはhandshakeがおわったということ
  ttLibC_RtmpCommandHandler2 *handler = (ttLibC_RtmpCommandHandler2 *)ctx->channel_handler;
  handler->bootstrap->sendConnect(); // コネクト命令を発行する
  return 0;
}

static tetty_errornum RtmpCommandHandler_channelRead(
    ttLibC_TettyContext *ctx,
    void *data,
    size_t data_size) {
  ttLibC_RtmpCommandHandler2 *handler       = (ttLibC_RtmpCommandHandler2 *)ctx->channel_handler;
  ttLibC_RtmpMessage         *rtmp_message  = (ttLibC_RtmpMessage *)data;
	ttLibC_ClientObject        *client_object = (ttLibC_ClientObject *)ctx->socket_info->ptr;
  switch(rtmp_message->header->message_type) {
  case RtmpMessageType_amf0Command:
    {
      ttLibC_Amf0Command *command = (ttLibC_Amf0Command *)rtmp_message;
      std::string command_name((const char *)command->command_name);
      if(command_name == "_result") {
        // resultの動作の場合
        ttLibC_Amf0Command *send_command = (ttLibC_Amf0Command *)ttLibC_StlMap_get(client_object->commandId_command_map, (void *)((long)command->command_id));
        // bootstrapにcallすればいいんですかね。
        handler->bootstrap->recvAmf0Result(send_command, command);
      }
      else if(command_name == "onStatus") {
        // statusのイベント
        handler->bootstrap->recvAmf0Command(command);
      }
      else {
        return ttLibC_TettyContext_super_channelRead(ctx, data, data_size);
      }
    }
    break;
  case RtmpMessageType_amf3Command:
    LOG_PRINT("amf3のコマンドを取得した。なんとか対処しなければならない。");
    break;
  default:
    // コマンド以外の処理は次のclientHandlerに処理を移譲する
    return ttLibC_TettyContext_super_channelRead(ctx, data, data_size);
  }
  return 0;
}

static tetty_errornum RtmpCommandHandler_write(
    ttLibC_TettyContext *ctx,
    void *data,
    size_t data_size) {
  ttLibC_RtmpMessage *message = (ttLibC_RtmpMessage *)data;
  switch(message->header->message_type) {
  case RtmpMessageType_amf0Command:
    {
      ttLibC_ClientObject *client_object = (ttLibC_ClientObject *)ctx->socket_info->ptr;
      ttLibC_Amf0Command *amf0_command = (ttLibC_Amf0Command *)message;
      if(amf0_command->command_id == -1) {
        // if command_id is -1(init value), apply id and wait for _result.
        amf0_command->command_id = client_object->next_command_id;
        ++ client_object->next_command_id; // increment for next command.
        // clone command for result check.
        ttLibC_Amf0Command *cloned_command = ttLibC_Amf0Command_make((const char *)amf0_command->command_name);
        cloned_command->command_id = amf0_command->command_id;
        if(amf0_command->obj1 != NULL) {
          cloned_command->obj1 = ttLibC_Amf0_clone(amf0_command->obj1);
        }
        if(amf0_command->obj2 != NULL) {
          cloned_command->obj2 = ttLibC_Amf0_clone(amf0_command->obj2);
        }
        cloned_command->promise = amf0_command->promise;
        ttLibC_StlMap_put(client_object->commandId_command_map, (void *)((long)amf0_command->command_id), cloned_command);
      }
      return ttLibC_TettyContext_super_write(ctx, data, data_size);
    }
    break;
  case RtmpMessageType_amf3Command:
    break;
  default:
    return ttLibC_TettyContext_super_write(ctx, data, data_size);
  }
  return 0;
}

ttLibC_RtmpCommandHandler *ttLibC_RtmpCommandHandler_make() {
  ttLibC_RtmpCommandHandler *handler = (ttLibC_RtmpCommandHandler *)ttLibC_malloc(sizeof(ttLibC_RtmpCommandHandler2));
  if(handler == NULL) {
    return NULL;
  }
  memset(handler, 0, sizeof(ttLibC_RtmpCommandHandler2));
  handler->channel_handler.channelActive = RtmpCommandHandler_channelActive;
  handler->channel_handler.channelRead = RtmpCommandHandler_channelRead;
  handler->channel_handler.write = RtmpCommandHandler_write;
  return handler;
}

ttLibC_RtmpCommandHandler *ttLibC_RtmpCommandHandler2_make(RtmpBootstrap *bootstrap) {
  ttLibC_RtmpCommandHandler2 *handler = (ttLibC_RtmpCommandHandler2 *)ttLibC_RtmpCommandHandler_make();
  handler->bootstrap = bootstrap;
  return (ttLibC_RtmpCommandHandler *)handler;
}

void ttLibC_RtmpCommandHandler_close(ttLibC_RtmpCommandHandler **handler) {
  ttLibC_RtmpCommandHandler *target = (ttLibC_RtmpCommandHandler *)*handler;
  if(target == NULL) {
    return;
  }
  ttLibC_free(target);
  *handler = NULL;
}
