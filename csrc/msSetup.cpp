﻿#include "msSetup.h"
#include "util.h"

#include <nan.h>
#ifdef __ENABLE_WIN32__
# include <ttLibC/util/msGlobalUtil.h>
#endif

static NAN_METHOD(CoInitialize) {
#ifdef __ENABLE_WIN32__
  ttLibC_MsGlobal_CoInitializeType type = CoInitializeType_normal;
  if(info.Length() >= 1) {
    std::string str_type(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0])));
    if(str_type == "multiThreaded") {
      type = CoInitializeType_multiThreaded;
    }
  }
  info.GetReturnValue().Set(ttLibC_MsGlobal_CoInitialize(type));
#else
  Nan::ThrowError(Nan::New("CoInitialize is for Win32 only.").ToLocalChecked());
#endif
}

static NAN_METHOD(CoUninitialize) {
#ifdef __ENABLE_WIN32__
  ttLibC_MsGlobal_CoUninitialize();
#else
  Nan::ThrowError(Nan::New("CoUninitialize is for Win32 only.").ToLocalChecked());
#endif
}

static NAN_METHOD(MFStartup) {
#ifdef __ENABLE_WIN32__
  info.GetReturnValue().Set(ttLibC_MsGlobal_MFStartup());
#else
  Nan::ThrowError(Nan::New("MFStartup is for Win32 only.").ToLocalChecked());
#endif
}

static NAN_METHOD(MFShutdown) {
#ifdef __ENABLE_WIN32__
  ttLibC_MsGlobal_MFShutdown();
#else
  Nan::ThrowError(Nan::New("MFShutdown is for Win32 only.").ToLocalChecked());
#endif
}

static NAN_METHOD(setlocale) {
#ifdef __ENABLE_WIN32__
  if(info.Length() < 0) {
    puts("coInitializeは引数が１つ必要です。");
    info.GetReturnValue().Set(false);
    return;
  }
  info.GetReturnValue().Set(ttLibC_MsGlobal_setlocale(*String::Utf8Value(v8::Isolate::GetCurrent(), ToString(info[0]))));
#else
  Nan::ThrowError(Nan::New("setlocale is for Win32 only.").ToLocalChecked());
#endif
}

void MsSetupInit(Local<Object> target) {
  Local<Object> obj = Nan::New<Object>();
  Nan::Set(obj, Nan::New("CoInitialize").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CoInitialize)).ToLocalChecked());
  Nan::Set(obj, Nan::New("CoUninitialize").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CoUninitialize)).ToLocalChecked());
  Nan::Set(obj, Nan::New("MFStartup").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(MFStartup)).ToLocalChecked());
  Nan::Set(obj, Nan::New("MFShutdown").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(MFShutdown)).ToLocalChecked());
  Nan::Set(obj, Nan::New("setlocale").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(setlocale)).ToLocalChecked());
  Nan::Set(target, Nan::New("MsSetup").ToLocalChecked(), obj);
}
