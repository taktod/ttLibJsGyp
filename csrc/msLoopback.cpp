#include "predef.h"
#include "msLoopback.h"
#include "frame.h"

#include <string>

void TTLIBJSGYP_CDECL MSLoopback::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Loopback").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "queryFrame", QueryFrame);
  Local<Function> func = Nan::GetFunction(tpl).ToLocalChecked();
  constructor().Reset(func);
  Nan::Set(func, Nan::New("listDevice").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(ListDevice)).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("MsLoopback").ToLocalChecked(),
    func);
}

NAN_METHOD(MSLoopback::New) {
#ifdef __ENABLE_WIN32__
  if(info.IsConstructCall()) {
    // パラメーターがおかしかったら例外を投げたいところ
    MSLoopback *loopback = new MSLoopback(info);
    loopback->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }
  else {
    Local<Value> *argv = new Local<Value>[info.Length()];
    for(int i = 0;i < info.Length();++ i) {
      argv[i] = info[i];
    }
    Local<Function> cons = Nan::New(constructor());
    info.GetReturnValue().Set(Nan::NewInstance(cons, (const int)info.Length(), argv).ToLocalChecked());
    delete[] argv;
  }
#else
  puts("windowsではない環境では、Loopbackは利用できません。");
#endif
}

bool MSLoopback::getDeviceCallback(void *ptr, const char *name) {
  Local<Value> *callback_ = (Local<Value> *)ptr;
  Nan::Callback callback((*callback_).As<Function>());
  Local<Value> args[] = {
    Nan::Null(),
    Nan::New(name).ToLocalChecked()
  };
  Local<Value> result = callback.Call(2, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

NAN_METHOD(MSLoopback::ListDevice) {
#ifdef __ENABLE_WIN32__
  // ここで文字列としてcallbackで応答しなければならないわけだが・・・
  // nodeでもcallbackにしとくか・・・
  if(info.Length() < 1) {
    info.GetReturnValue().Set(false);
    return;
  }
  // info[0]はcallbackであるべき。
  info.GetReturnValue().Set(
    ttLibC_MmAudioLoopback_getDeviceNames(
      getDeviceCallback,
      &info[0])
  );
#else
  info.GetReturnValue().Set(true);
#endif
}

NAN_METHOD(MSLoopback::QueryFrame) {
  bool result = false;
#ifdef __ENABLE_WIN32__
  MSLoopback *loopback = Nan::ObjectWrap::Unwrap<MSLoopback>(info.Holder());
  if(loopback != NULL) {
    loopback->callback_ = info[0];
    result = ttLibC_MmAudioLoopback_queryFrame(loopback->loopback_, captureCallback, loopback);
  }
#endif
  info.GetReturnValue().Set(result);
}

bool MSLoopback::captureCallback(void *ptr, ttLibC_PcmS16 *pcm) {
  MSLoopback *loopback = (MSLoopback *)ptr;
  Nan::Callback callback(loopback->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(loopback->jsPcmFrame_);
  Frame::setFrame(jsFrame, (ttLibC_Frame *)pcm);
  Local<Value> args[] = {
    Nan::Null(),
    jsFrame
  };
  Local<Value> result = callback.Call(2, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

MSLoopback::MSLoopback(Nan::NAN_METHOD_ARGS_TYPE info) {
#ifdef __ENABLE_WIN32__
  std::string locale("");
  std::string device("");
  if(info.Length() >= 1) {
    locale = std::string(*String::Utf8Value(info[0]->ToString()));
  }
  if(info.Length() == 2) {
    // 2の場合はdeviceの指定があるもんだとして動作する。
    device = std::string(*String::Utf8Value(info[1]->ToString()));
  }
  char *c_locale = NULL;
  char *c_device = NULL;
  if(locale.length() != 0) {
    c_locale = (char *)locale.c_str();
  }
  if(device.length() != 0) {
    c_device = (char *)device.c_str();
  }
  loopback_ = ttLibC_MmAudioLoopback_make(
    (const char *)c_locale,
    (const char *)c_device);
  jsPcmFrame_.Reset(Frame::newInstance());
#endif
}

MSLoopback::~MSLoopback() {
#ifdef __ENABLE_WIN32__
  ttLibC_MmAudioLoopback_close(&loopback_);
  jsPcmFrame_.Reset();
#endif
}
