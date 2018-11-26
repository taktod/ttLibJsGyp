#include "predef.h"
#include "decoder.h"
#include "frame.h"

#include "decoder/audioConverterDecoder.h"
#include "decoder/avcodecDecoder.h"
#include "decoder/jpegDecoder.h"
#include "decoder/mp3lameDecoder.h"
#include "decoder/openh264Decoder.h"
#include "decoder/opusDecoder.h"
#include "decoder/pngDecoder.h"
#include "decoder/speexDecoder.h"
#include "decoder/theoraDecoder.h"
#include "decoder/vorbisDecoder.h"
#include "decoder/vtDecompressSessionDecoder.h"

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
  SetPrototypeMethod(tpl, "getCodecControl", GetCodecControl);
  Local<Function> func = Nan::GetFunction(tpl).ToLocalChecked();
  constructor().Reset(func);
  Nan::Set(func, Nan::New("check").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CheckAvailable)).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Decoder").ToLocalChecked(),
    func);
}

static bool checkAvailable(std::string type) {
  bool result = false;
  if(type == "audioConverter") {
#ifdef __ENABLE_APPLE__
    result = true;
#endif
  }
  else if(type == "avcodec") {
#ifdef __ENABLE_AVCODEC__
    result = true;
#endif
  }
  else if(type == "jpeg") {
#ifdef __ENABLE_JPEG__
    result = true;
#endif
  }
  else if(type == "mp3lame") {
#ifdef __ENABLE_MP3LAME_DECODE__
    result = true;
#endif
  }
  else if(type == "openh264") {
#ifdef __ENABLE_OPENH264__
    result = true;
#endif
  }
  else if(type == "opus") {
#ifdef __ENABLE_OPUS__
    result = true;
#endif
  }
  else if(type == "png") {
#ifdef __ENABLE_LIBPNG__
    result = true;
#endif
  }
  else if(type == "speex") {
#ifdef __ENABLE_SPEEX__
    result = true;
#endif
  }
  else if(type == "theora") {
#ifdef __ENABLE_THEORA__
    result = true;
#endif
  }
  else if(type == "vorbis") {
#ifdef __ENABLE_VORBIS_DECODE__
    result = true;
#endif
  }
  else if(type == "vtDecompressSession") {
#ifdef __ENABLE_APPLE__
    result = true;
#endif
  }
  return result;
}

NAN_METHOD(Decoder::CheckAvailable) {
  bool result = false;
  if(info.Length() > 0) {
    std::string type(*String::Utf8Value(info[0]->ToString()));
    result = checkAvailable(type);
  }
  info.GetReturnValue().Set(result);
}
NAN_METHOD(Decoder::New) {
  std::string type(*String::Utf8Value(info[0]->ToString()));
  if(!checkAvailable(type)) {
    Nan::ThrowError(Nan::New(type + " decoder is not available.").ToLocalChecked());
    return;
  }
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    Decoder *decoder = NULL;
    if(type == "audioConverter") {
      decoder = new AudioConverterDecoder(info[1]->ToObject());
    }
    else if(type == "avcodec") {
      decoder = new AvcodecDecoder(info[1]->ToObject());
    }
    else if(type == "jpeg") {
      decoder = new JpegDecoder(info[1]->ToObject());
    }
    else if(type == "mp3lame") {
      decoder = new Mp3lameDecoder(info[1]->ToObject());
    }
    else if(type == "openh264") {
      decoder = new Openh264Decoder(info[1]->ToObject());
    }
    else if(type == "opus") {
      decoder = new OpusDecoder_(info[1]->ToObject());
    }
    else if(type == "png") {
      decoder = new PngDecoder(info[1]->ToObject());
    }
    else if(type == "speex") {
      decoder = new SpeexDecoder(info[1]->ToObject());
    }
    else if(type == "theora") {
      decoder = new TheoraDecoder(info[1]->ToObject());
    }
    else if(type == "vorbis") {
      decoder = new VorbisDecoder(info[1]->ToObject());
    }
    else if(type == "vtDecompressSession") {
      decoder = new VtDecompressSessionDecoder(info[1]->ToObject());
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
  if(info.Length() == 2) {
    Decoder *decoder = Nan::ObjectWrap::Unwrap<Decoder>(info.Holder());
    if(decoder == NULL) {
      puts("decoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(decoder->type_ == gdt_opus) {
      OpusDecoder_ *opusDecoder = (OpusDecoder_ *)decoder;
      info.GetReturnValue().Set(opusDecoder->codecControl(
        std::string(*String::Utf8Value(info[0]->ToString())),
        info[1]->Uint32Value()));
      return;
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Decoder::GetCodecControl) {
  if(info.Length() == 1) {
    Decoder *decoder = Nan::ObjectWrap::Unwrap<Decoder>(info.Holder());
    if(decoder == NULL) {
      puts("decoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    if(decoder->type_ == gdt_opus) {
      OpusDecoder_ *opusDecoder = (OpusDecoder_ *)decoder;
      info.GetReturnValue().Set(opusDecoder->codecControl(
        std::string(*String::Utf8Value(info[0]->ToString())),
        0));
      return;
    }
  }
  info.GetReturnValue().Set(false);
}

Decoder::Decoder() {
  jsFrame_.Reset(Frame::newInstance());
}
