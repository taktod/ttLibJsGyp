#include "predef.h"
#include "decoder.h"
#include "frame.h"

#include "decoder/avcodec.h"

#include <string>

class DummyDecoder : public Decoder {
public:
  DummyDecoder() : Decoder() {}
  bool decode(ttLibC_Frame *frame) {
    return false;
  }
private:
  ~DummyDecoder() {}
};

void TTLIBJSGYP_CDECL Decoder::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Decoder").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "decode",          Decode);
  SetPrototypeMethod(tpl, "setCodecControl", SetCodecControl);
  Local<Function> func = Nan::GetFunction(tpl).ToLocalChecked();
  constructor().Reset(func);
  Nan::Set(func, Nan::New("check").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CheckAvailable)).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Decoder").ToLocalChecked(),
    func);
}

NAN_METHOD(Decoder::CheckAvailable) {
  bool result = false;
  if(info.Length() > 0) {
    std::string type(*String::Utf8Value(info[0]->ToString()));
    if(type == "avcodec") {
#ifdef __ENABLE_AVCODEC__
      result = true;
#endif
    }
  }
  info.GetReturnValue().Set(result);
}
NAN_METHOD(Decoder::New) {
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    std::string type(*String::Utf8Value(info[0]->ToString()));
    Decoder *decoder = NULL;
    if(type == "avcodec") {
      decoder = new AvcodecDecoder(info[1]->ToObject());
    }
    else {
      printf("%sは未定義です。\n", type.c_str());
      decoder = new DummyDecoder();
    }
    if(decoder != NULL) {
      decoder->Wrap(info.This());
    }
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
}

NAN_METHOD(Decoder::Decode) {
  if(info.Length() == 2) {
    Decoder *decoder = Nan::ObjectWrap::Unwrap<Decoder>(info.Holder());
    if(decoder == NULL) {
      puts("decoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    decoder->callback_ = info[1];
    info.GetReturnValue().Set(
      decoder->decode(Frame::refFrame(info[0]))
    );
    return;
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Decoder::SetCodecControl) {

}

Decoder::Decoder() {
  jsFrame_.Reset(Frame::newInstance());
}
