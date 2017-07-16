#include "vtCompressSession.h"
#include "../frame.h"
#include <ttLibC/frame/video/h264.h>

VtCompressSessionEncoder::VtCompressSessionEncoder(Local<Object> params) : Encoder() {
  type_ = get_vtCompressSession;
#ifdef __ENABLE_APPLE__
  ttLibC_Frame_Type frameType = Frame::getFrameType(
    std::string(*String::Utf8Value(
      Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked()->ToString()))
  );
  uint32_t width   = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t height  = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t fps     = Nan::Get(params, Nan::New("fps").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t bitrate = Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  bool isBaseline  = Nan::Get(params, Nan::New("isBaseline").ToLocalChecked()).ToLocalChecked()->IsTrue();
  encoder_ = ttLibC_VtEncoder_make_ex(width, height, fps, bitrate, isBaseline, frameType);

  frameStack_ = ttLibC_StlList_make();
  pthread_mutex_init(&frameMutex_, NULL);
#endif
}

static bool clearFrameStack(void *ptr, void *item) {
  if(item != NULL) {
    ttLibC_Frame *frame = (ttLibC_Frame *)item;
    ttLibC_Frame_close(&frame);
  }
  return true;
}

VtCompressSessionEncoder::~VtCompressSessionEncoder() {
#ifdef __ENABLE_APPLE__
  ttLibC_VtEncoder_close(&encoder_);
  pthread_mutex_destroy(&frameMutex_);
  ttLibC_StlList_forEach(frameStack_, clearFrameStack, NULL);
  ttLibC_StlList_close(&frameStack_);
#endif
}

bool VtCompressSessionEncoder::encodeCallback(void *ptr, ttLibC_Video *video) {
#ifdef __ENABLE_APPLE__
  VtCompressSessionEncoder *encoder = (VtCompressSessionEncoder *)ptr;
  // ここからcallするとsegfaultが発生するの？どういうこと？
  // ということは、こうじゃなくて、AsyncWorkerのスレッドでvtCompressSessionとか呼び出しを実施するように改良しないとだめだな。
  int r = pthread_mutex_lock(&encoder->frameMutex_);
  if(r != 0) {
    puts("lockエラー");
  }
  ttLibC_StlList_addLast(encoder->frameStack_, ttLibC_Frame_clone(NULL, (ttLibC_Frame *)video));
  // なんか処理する。
  r = pthread_mutex_unlock(&encoder->frameMutex_);
  if(r != 0) {
    puts("unlockエラー");
  }
#endif
  return true;
}

bool VtCompressSessionEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_APPLE__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame->type != frameType_yuv420) {
    puts("yuv420のみ処理可能です。");
    return false;
  }
  ttLibC_VtEncoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
  int r = pthread_mutex_lock(&frameMutex_);
  if(r != 0) {
    puts("lockエラー2");
  }
  Nan::Callback callback(callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(jsFrame_);
  bool result = true;
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
  // ここでstackにあるデータをすべてcallbackで応答します。
  // なんか処理する。
  r = pthread_mutex_unlock(&frameMutex_);
  if(r != 0) {
    puts("unlockエラー2");
  }
  return result;
#else
  return false;
#endif
}
