#ifndef CSRC_UTIL_H
#define CSRC_UTIL_H

#include <nan.h>

using namespace v8;

Local<Value> callbackCall(Nan::Callback &callback, int num, Local<Value> *args);
Local<Object> ToObject(Local<Value> value);
uint32_t Uint32Value(Local<Value> value);
double NumberValue(Local<Value> value);
Local<String> ToString(Local<Value> value);

#endif
