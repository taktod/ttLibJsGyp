// 全体のvalueを定義する動作
#include <nan.h>

#include <ttLibC/container/container.h>

using namespace v8;

static NAN_MODULE_INIT(Init) {
  Local<Object> value = Nan::New<Object>();
  Nan::Set(value, Nan::New("splitTypeKey").ToLocalChecked(),    Nan::New(containerWriter_keyFrame_split));
  Nan::Set(value, Nan::New("splitTypeInner").ToLocalChecked(),  Nan::New(containerWriter_innerFrame_split));
  Nan::Set(value, Nan::New("splitTypeP").ToLocalChecked(),      Nan::New(containerWriter_pFrame_split));
  Nan::Set(value, Nan::New("splitTypeDB").ToLocalChecked(),     Nan::New(containerWriter_disposableBFrame_split));
  Nan::Set(value, Nan::New("splitTypeB").ToLocalChecked(),      Nan::New(containerWriter_bFrame_split));
  Nan::Set(value, Nan::New("splitTypeAllKey").ToLocalChecked(), Nan::New(containerWriter_allKeyFrame_split));
  Nan::Set(
    target,
    Nan::New("ValueDefine").ToLocalChecked(),
    value
  );
}

NODE_MODULE(valueDefine, Init);
