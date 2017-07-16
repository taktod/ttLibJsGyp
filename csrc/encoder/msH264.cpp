#include "../predef.h"
#include "msH264.h"
#include "../frame.h"
#include <ttLibC/frame/video/yuv420.h>

#include <ttLibC/allocator.h>

void TTLIBJSGYP_CDECL MSH264Encoder::classInit(Local<Object> target) {
  Local<Object> obj = Nan::New<Object>();
  Nan::Set(obj, Nan::New("listEncoders").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(ListEncoders)).ToLocalChecked());
  Nan::Set(target, Nan::New("MsH264").ToLocalChecked(), obj);
}

NAN_METHOD(MSH264Encoder::ListEncoders) {
#ifdef __ENABLE_WIN32__
  if(info.Length() < 1) {
    info.GetReturnValue().Set(false);
    return;
  }
  info.GetReturnValue().Set(
    ttLibC_MsH264Encoder_listEncoders(listEncoderCallback, &info[0])
  );
#endif
}

bool MSH264Encoder::listEncoderCallback(void *ptr, const char *name) {
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

MSH264Encoder::MSH264Encoder(Local<Object> params) : Encoder() {
  type_ = get_msH264;
#ifdef __ENABLE_WIN32__
  // ここで初期化を適当に実施してみようと思う。
  uint32_t width   = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t height  = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t bitrate = Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  std::string name(*String::Utf8Value(Nan::Get(params, Nan::New("encoder").ToLocalChecked()).ToLocalChecked()->ToString()));
  // とりあえずEnumで必要なデータの取得はできたので、このまま進める。
  // こんな感じかな。該当エンコーダーをここで指定して、そこに対してencodeを実施する的な感じで
  encoder_ = ttLibC_MsH264Encoder_make(name.c_str(), width, height, bitrate);
  // 細かい設定も可能にしておきたいところだが・・・
  // あとで考えよう

  frameStack_ = ttLibC_StlList_make();
  // ここでmutexを獲得しとく
  hMutex_ = CreateMutex(0, FALSE, 0);
#endif
}

#ifdef __ENABLE_WIN32__
static bool clearFrameStack(void *ptr, void *item) {
  if(item != NULL) {
    ttLibC_Frame *frame = (ttLibC_Frame *)item;
    ttLibC_Frame_close(&frame);
  }
  return true;
}
#endif

MSH264Encoder::~MSH264Encoder() {
#ifdef __ENABLE_WIN32__
  ttLibC_MsH264Encoder_close(&encoder_);
  CloseHandle(hMutex_);
  ttLibC_StlList_forEach(frameStack_, clearFrameStack, NULL);
  ttLibC_StlList_close(&frameStack_);
#endif
}

bool MSH264Encoder::encodeCallback(void *ptr, ttLibC_H264 *h264) {
#ifdef __ENABLE_WIN32__
  MSH264Encoder *encoder = (MSH264Encoder *)ptr;
  WaitForSingleObject(encoder->hMutex_, INFINITE);
  // ここでなにかしておく
  ttLibC_StlList_addLast(encoder->frameStack_, ttLibC_H264_clone(NULL, h264));
  ReleaseMutex(encoder->hMutex_);
#endif
  return true;
}

bool MSH264Encoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_WIN32__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
  }
  if(frame->type != frameType_yuv420) {
    puts("yuv420のみ処理可能です。");
    return false;
  }
  bool result = ttLibC_MsH264Encoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
  if(!result) {
    return false;
  }
  WaitForSingleObject(hMutex_, INFINITE);
  // ここでなにかしておく
  Nan::Callback callback(callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(jsFrame_);
  do {
    ttLibC_Frame *frame = (ttLibC_Frame *)ttLibC_StlList_refFirst(frameStack_);
    if(frame != NULL) {
      ttLibC_StlList_remove(frameStack_, frame);

      Frame::setFrame(jsFrame, frame);
      Local<Value> args[] = {
        Nan::Null(),
        jsFrame
      };
      Local<Value> jsResult = callback.Call(2, args);
      if(!jsResult->IsTrue()) {
        if(jsResult->IsUndefined()) {
          puts("応答が設定されていません。");
        }
        result = false;
        break;
      }
      ttLibC_Frame_close(&frame);
    }
  } while(frameStack_->size != 0);
  ReleaseMutex(hMutex_);
  return result;
#else
  return false;
#endif
}
