#include "util.h"

Local<Value> callbackCall(Nan::Callback &callback, int num, Local<Value> *args) {
  return callback.Call(num, args);
}

Local<Object> ToObject(Local<Value> value) {
  return value->ToObject();
}

uint32_t Uint32Value(Local<Value> value) {
  return value->Uint32Value();
}

double NumberValue(Local<Value> value) {
  return value->NumberValue();
}

Local<String> ToString(Local<Value> value) {
  return value->ToString();
}
