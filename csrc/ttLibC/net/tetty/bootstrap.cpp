// tettyのbootstrapのnode実装
// ここにnodeからアクセスすることにしようと思う。

#include "../../../predef.h"
#include <ttLibC/net/tetty/bootstrap.h>
#include <ttLibC/net/tetty/promise.h>

#include "bootstrap.h"
#include <ttLibC/allocator.h>
#include <ttLibC/util/stlListUtil.h>

#include <ttLibC/net/udp.h>

Bootstrap::Bootstrap() {
  bootstrap_ = ttLibC_TettyBootstrap_make();
}

Bootstrap::~Bootstrap() {
  ttLibC_TettyBootstrap_close(&bootstrap_);
}

ttLibC_TettyBootstrap *ttLibC_TettyBootstrap_make() {
  ttLibC_TettyBootstrap_ *bootstrap = (ttLibC_TettyBootstrap_ *)ttLibC_malloc(sizeof(ttLibC_TettyBootstrap_));
  if(bootstrap == NULL) {
    return NULL;
  }
  bootstrap->channel_type = ChannelType_Tcp;
  bootstrap->inherit_super.channel_type = ChannelType_Tcp;
  bootstrap->pipeline = ttLibC_StlList_make();
  bootstrap->socket_info = NULL;
  bootstrap->inherit_super.socket_info = NULL;
  bootstrap->tcp_client_info_list = ttLibC_StlList_make();
  bootstrap->so_keepalive = false;
  bootstrap->so_reuseaddr = false;
  bootstrap->tcp_nodelay = false;
  bootstrap->inherit_super.error_number = 0;
  bootstrap->close_future = NULL;
  bootstrap->fdset = NULL;
  bootstrap->fdchkset = NULL;
  return (ttLibC_TettyBootstrap *)bootstrap;
}

bool ttLibC_TettyBootstrap_channel(
    ttLibC_TettyBootstrap *bootstrap,
    ttLibC_Tetty_ChannelType channel_type) {
  // tcpのみ、特に実装してどうこうというのはないはず。
  if(channel_type != ChannelType_Tcp) {
    puts("設定可能チャンネルはtcpのみです。いまのところ");
    return false;
  }
  ttLibC_TettyBootstrap_ *bootstrap_ = (ttLibC_TettyBootstrap_ *)bootstrap;
  bootstrap_->channel_type = channel_type;
  bootstrap_->inherit_super.channel_type = channel_type;
  return true;
}

bool ttLibC_TettyBootstrap_option(
    ttLibC_TettyBootstrap *bootstrap,
    ttLibC_Tetty_Option option) {
  // tcpNoDelayとkeepAliveが必要だが、デフォルトでonにするので、放置
  return false;
}

bool ttLibC_TettyBootstrap_bind(
    ttLibC_TettyBootstrap *bootstrap,
    int port) {
  // サーバー動作はつくらない。
  return false;
}

bool ttLibC_TettyBootstrap_connect(
    ttLibC_TettyBootstrap *bootstrap,
    const char *host,
    int port) {
  puts("使いません。代わりにconnectNodeを使います。");
  return false;
}

bool ttLibC_TettyBootstrap_connectNode(
    ttLibC_TettyBootstrap *bootstrap,
    Local<Object> socket) {
  ttLibC_TettyBootstrap_ *bootstrap_ = (ttLibC_TettyBootstrap_ *)bootstrap;
  if(bootstrap_->channel_type == ChannelType_Udp) {
    ERR_PRINT("connect is not support in udp.");
    return false;
  }
  // とりあえず空のclientInfoで十分
  ttLibC_TcpClientInfo *client_info = (ttLibC_TcpClientInfo *)ttLibC_malloc(sizeof(ttLibC_TcpClientInfo));
  memset(client_info, 0, sizeof(ttLibC_TcpClientInfo));
  // Local<Object>は即蒸発するので、Persistentとしてrefを持っとく
  Nan::Persistent<Object> *object = new Nan::Persistent<Object>();
  object->Reset(socket);
  client_info->inherit_super.addr = (ttLibC_SockaddrIn *)object;
  // クライアントinfoを後ろにくっつける。
  ttLibC_StlList_addLast(bootstrap_->tcp_client_info_list, client_info);
  ttLibC_TettyContext_connect_(bootstrap, (ttLibC_SocketInfo *)client_info);
  ttLibC_TettyContext_channelActive_(bootstrap, (ttLibC_SocketInfo *)client_info);
  return true;
}

bool ttLibC_TettyBootstrap_update(
    ttLibC_TettyBootstrap *bootstrap,
    uint32_t wait_interval) {
  return true;
}

void ttLibC_TettyBootstrap_closeServer(ttLibC_TettyBootstrap *bootstrap) {
  // serverを閉じる。たぶんいらない。
}

static bool TettyBootstrap_closeClientCallback(void *ptr, void *item) {
  (void)ptr;
  if(item != NULL) {
    ttLibC_TcpClientInfo *client_info = (ttLibC_TcpClientInfo *)item;
    delete client_info->inherit_super.addr;
    client_info->inherit_super.addr = NULL;
    ttLibC_DynamicBuffer_close(&client_info->write_buffer);
    ttLibC_free(client_info);
  }
  return true;
}

void ttLibC_TettyBootstrap_closeClients(ttLibC_TettyBootstrap *bootstrap) {
  ttLibC_TettyBootstrap_ *bootstrap_ = (ttLibC_TettyBootstrap_ *)bootstrap;
  if(bootstrap_->tcp_client_info_list != NULL) {
    ttLibC_StlList_forEach(bootstrap_->tcp_client_info_list, TettyBootstrap_closeClientCallback, NULL);
  }
}

void ttLibC_TettyBootstrap_close(ttLibC_TettyBootstrap **bootstrap) {
  // bootstrapをcloseする
  ttLibC_TettyBootstrap_ *target = (ttLibC_TettyBootstrap_ *)*bootstrap;
  if(target == NULL) {
    return;
  }
  ttLibC_TettyBootstrap_closeClients((ttLibC_TettyBootstrap *)target);
  ttLibC_TettyBootstrap_closeServer((ttLibC_TettyBootstrap *)target);
  ttLibC_StlList_close(&target->tcp_client_info_list);
  ttLibC_StlList_close(&target->pipeline);
  // fdsetのcloseは必要ない。
  if(target->close_future != NULL) {
    puts("close futureを使うのは想定外");
  }
  ttLibC_free(target);
  *bootstrap = NULL;
}

void ttLibC_TettyBootstrap_pipeline_addLast(
    ttLibC_TettyBootstrap *bootstrap,
    ttLibC_TettyChannelHandler *channel_handler) {
  // 必要
  ttLibC_TettyBootstrap_ *bootstrap_ = (ttLibC_TettyBootstrap_ *)bootstrap;
  ttLibC_StlList_addLast(bootstrap_->pipeline, channel_handler);
}

void ttLibC_TettyBootstrap_pipeline_remove(
    ttLibC_TettyBootstrap *bootstrap,
    ttLibC_TettyChannelHandler *channel_handler) {
  // つくるけど多分使わない
  ttLibC_TettyBootstrap_ *bootstrap_ = (ttLibC_TettyBootstrap_ *)bootstrap;
  ttLibC_StlList_remove(bootstrap_->pipeline, channel_handler);
}

tetty_errornum ttLibC_TettyBootstrap_pipeline_fireUserEventTriggered(
    ttLibC_TettyBootstrap *bootstrap,
    ttLibC_SocketInfo *socket_info,
    void *data,
    size_t data_size) {
  // ユーザーイベントを使うかどうかだね。
  // 今の所使わないので、放置
  return 0;
}

static bool TettyBootstrap_fireChannelReadNodeCallback(void *ptr, void *item) {
  ttLibC_TettyContext_ *ctx = (ttLibC_TettyContext_ *)ptr;
  if(item != NULL) {
    ttLibC_TcpClientInfo *client_info = (ttLibC_TcpClientInfo *)item;
    Nan::Persistent<Object> *object = (Nan::Persistent<Object> * )client_info->inherit_super.addr;
    Local<Object> lo = Nan::New(*object);
    int hashCode = lo->GetIdentityHash();
    if(ctx->error_no == hashCode) {
      // このクライアントについて、chanelReadを実行しなければ・・・
      ttLibC_TettyContext_channelRead_(ctx->bootstrap, (ttLibC_SocketInfo *)client_info, ctx->data, ctx->data_size);
      return false;
    }
  }
  return true;
}

tetty_errornum ttLibC_TettyBootstrap_fireChannelReadNode(
    ttLibC_TettyBootstrap *bootstrap,
    Local<Object> socket,
    void *data,
    size_t data_size) {
  ttLibC_TettyBootstrap_ *bootstrap_ = (ttLibC_TettyBootstrap_ *)bootstrap;
  // client_infoをループさせてsocketが一致するものをcallすべきだと思うけど・・・
  // error_noにhash値をいれて渡すことにする。アレな実装だけど、とりあえずこれで
  ttLibC_TettyContext_ ctx;
  ctx.bootstrap = bootstrap;
  ctx.data = data;
  ctx.data_size = data_size;
  ctx.error_no = socket->GetIdentityHash();
  // hash値が一致するものを探し出す。
  ttLibC_StlList_forEach(bootstrap_->tcp_client_info_list, TettyBootstrap_fireChannelReadNodeCallback, &ctx);
  return 0;
}

tetty_errornum ttLibC_TettyBootstrap_channels_write(
    ttLibC_TettyBootstrap *bootstrap,
    void *data,
    size_t data_size) {
  if(bootstrap->error_number != 0) {
    return bootstrap->error_number;
  }
  return ttLibC_TettyContext_channel_write_(
      bootstrap,
      NULL,
      data,
      data_size);
}

static bool TettyBootstrap_channelEach_write_callback(void *ptr, void *item) {
  ttLibC_TettyContext_ *ctx = (ttLibC_TettyContext_ *)ptr;
  ttLibC_TcpClientInfo *client_info = (ttLibC_TcpClientInfo *)item;
  ttLibC_TettyContext_channel_write_(ctx->bootstrap, (ttLibC_SocketInfo *)client_info, ctx->data, ctx->data_size);
  return true;
}

tetty_errornum ttLibC_TettyBootstrap_channelEach_write(
    ttLibC_TettyBootstrap *bootstrap,
    void *data,
    size_t data_size) {
  if(bootstrap->error_number != 0) {
    return bootstrap->error_number;
  }
  ttLibC_TettyContext_ ctx;
  ctx.bootstrap = bootstrap;
  ctx.data = data;
  ctx.data_size = data_size;
  ttLibC_TettyBootstrap_ *bootstrap_ = (ttLibC_TettyBootstrap_ *)bootstrap;
  ttLibC_StlList_forEach(bootstrap_->tcp_client_info_list, TettyBootstrap_channelEach_write_callback, &ctx);
  return 0;
}

static bool TettyBootstrap_channelEach_callPipelineFlush_callback(void *ptr, void *item) {
  // call pipeline->flush
  ttLibC_TettyContext_flush_((ttLibC_TettyBootstrap *)ptr, (ttLibC_SocketInfo *)item);
  return true;
}

static bool TettyBootstrap_channelEach_flush_callback(void *ptr, void *item) {
  (void)ptr;
  ttLibC_TcpClientInfo *client_info = (ttLibC_TcpClientInfo *)item;
  if(client_info->inherit_super.addr == NULL) {
    puts("addrがnullで動作できない。");
  }
  else {
    ttLibC_DynamicBuffer * write_buffer = client_info->write_buffer;
    if(ttLibC_DynamicBuffer_refSize(write_buffer) == 0) {
      // 書き出すデータがない。
      return true;
    }
    // socketをキックする。
    // ただし直接writeをkickしたらちゃんと動作しなかった。
    Nan::Persistent<Object> *object = (Nan::Persistent<Object> *)client_info->inherit_super.addr;
    Local<Value> write = Nan::Get(Nan::New(*object), Nan::New("tettyWrite").ToLocalChecked()).ToLocalChecked();
    if(write->IsFunction()) {
      Nan::Callback writeCall(write.As<Function>());
      Local<Value> args[] = {
        Nan::CopyBuffer(
          (char *)ttLibC_DynamicBuffer_refData(write_buffer),
          ttLibC_DynamicBuffer_refSize(write_buffer)).ToLocalChecked()
      };
      writeCall.Call(1, args);
      ttLibC_DynamicBuffer_empty(write_buffer);
    }
  }
  return true;
}

tetty_errornum ttLibC_TettyBootstrap_channels_flush(ttLibC_TettyBootstrap *bootstrap) {
  ttLibC_TettyBootstrap_ *bootstrap_ = (ttLibC_TettyBootstrap_ *)bootstrap;
  // flushを全体に実行する。
  ttLibC_StlList_forEach(bootstrap_->tcp_client_info_list, TettyBootstrap_channelEach_callPipelineFlush_callback, bootstrap);
  // 実際のwriteをすべてのクライアントに対して実行する。
  ttLibC_StlList_forEach(bootstrap_->tcp_client_info_list, TettyBootstrap_channelEach_flush_callback, NULL);
  return 0;
}

tetty_errornum ttLibC_TettyBootstrap_channels_writeAndFlush(
    ttLibC_TettyBootstrap *bootstrap,
    void *data,
    size_t data_size) {
  tetty_errornum error_num = ttLibC_TettyBootstrap_channels_write(bootstrap, data, data_size);
  if(error_num != 0) {
    return error_num;
  }
  error_num = ttLibC_TettyBootstrap_channels_flush(bootstrap);
  return error_num;
}

tetty_errornum ttLibC_TettyBootstrap_channelEach_writeAndFlush(
    ttLibC_TettyBootstrap *bootstrap,
    void *data,
    size_t data_size) {
  tetty_errornum error_num = ttLibC_TettyBootstrap_channelEach_write(
      bootstrap, data, data_size);
  if(error_num != 0) {
    return error_num;
  }
  error_num = ttLibC_TettyBootstrap_channels_flush(bootstrap);
  return error_num;
}

ttLibC_TettyPromise *ttLibC_TettyBootstrap_makePromise(ttLibC_TettyBootstrap *bootstrap) {
  if(bootstrap->error_number != 0) {
    return NULL;
  }
  return ttLibC_TettyPromise_make_(bootstrap);
}

ttLibC_TettyFuture *ttLibC_TettyBootstrap_closeFuture(ttLibC_TettyBootstrap *bootstrap) {
  return NULL;
}

// ttLibC/net/tetty/bootstrap.hより
bool ttLibC_TettyBootstrap_closeClient_(
    ttLibC_TettyBootstrap *bootstrap,
    ttLibC_SocketInfo *socket_info) {return false;}

// ttLibC/net/udp.hより
ttLibC_UdpSocketInfo *ttLibC_UdpSocket_make(uint16_t port) {return NULL;}
bool ttLibC_UdpSocket_open(ttLibC_UdpSocketInfo *socket_info) {return false;}
bool ttLibC_UdpSocket_write(
    ttLibC_UdpSocketInfo *socket_info,
    ttLibC_DatagramPacket *packet) {return false;}
int64_t ttLibC_UdpSocket_read(
    ttLibC_UdpSocketInfo *socket_info,
    ttLibC_DatagramPacket *packet) {return -1;}
void ttLibC_UdpSocket_close(ttLibC_UdpSocketInfo **socket_info) {}
ttLibC_DatagramPacket *ttLibC_DatagramPacket_make(
    void *data,
    size_t data_size) {return NULL;}
ttLibC_DatagramPacket *ttLibC_DatagramPacket_makeWithTarget(
    void *data,
    size_t data_size,
    const char *target_address,
    int16_t target_port) {return NULL;}
void ttLibC_DatagramPacket_close(ttLibC_DatagramPacket **packet) {}
