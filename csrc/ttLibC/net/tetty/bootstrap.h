#ifndef CSRC_TTLIBC_NET_TETTY_BOOTSTRAP_H
#define CSRC_TTLIBC_NET_TETTY_BOOTSTRAP_H

#include <nan.h>
#include <ttLibC/net/tetty.h>

using namespace v8;

class Bootstrap : public Nan::ObjectWrap {
protected:
  Bootstrap();
  virtual ~Bootstrap();
  ttLibC_TettyBootstrap *bootstrap_;
};

bool ttLibC_TettyBootstrap_connectNode(ttLibC_TettyBootstrap *bootstrap, Local<Object> socket);
tetty_errornum ttLibC_TettyBootstrap_fireChannelReadNode(
    ttLibC_TettyBootstrap *bootstrap,
    Local<Object> socket,
    void *data,
    size_t data_size);

#endif
