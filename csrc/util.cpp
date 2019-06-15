#include "util.h"

Local<Value> callbackCall(Nan::Callback &callback, int num, Local<Value> *args) {
  return callback.Call(num, args);
}

Local<Object> ToObject(Local<Value> value) {
  return Nan::To<v8::Object>(value).ToLocalChecked();
}

uint32_t Uint32Value(Local<Value> value) {
  return Nan::To<uint32_t>(value).FromJust();
}

double NumberValue(Local<Value> value) {
  return Nan::To<double>(value).FromJust();
}

Local<String> ToString(Local<Value> value) {
  return Nan::To<v8::String>(value).ToLocalChecked();
}
