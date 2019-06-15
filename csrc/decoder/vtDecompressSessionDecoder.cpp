#include "vtDecompressSessionDecoder.h"
#include "../frame.h"
#include "../util.h"

VtDecompressSessionDecoder::VtDecompressSessionDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_vtDecompressSession;
#ifdef __ENABLE_APPLE__
  ttLibC_Frame_Type frameType = Frame::getFrameType(
    std::string(*String::Utf8Value(v8::Isolate::GetCurrent(),
      ToString(Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked())))
  );
  decoder_ = ttLibC_VtDecoder_make(frameType);
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

VtDecompressSessionDecoder::~VtDecompressSessionDecoder() {
#ifdef __ENABLE_APPLE__
  ttLibC_VtDecoder_close(&decoder_);
  pthread_mutex_destroy(&frameMutex_);
  ttLibC_StlList_forEach(frameStack_, clearFrameStack, NULL);
  ttLibC_StlList_close(&frameStack_);
#endif
}

bool VtDecompressSessionDecoder::decodeCallback(void *ptr, ttLibC_Yuv420 *yuv) {
#ifdef __ENABLE_APPLE__
  VtDecompressSessionDecoder *decoder = (VtDecompressSessionDecoder *)ptr;
  int r = pthread_mutex_lock(&decoder->frameMutex_);
  if(r != 0) {
    puts("lockエラー");
  }
  ttLibC_StlList_addLast(decoder->frameStack_, ttLibC_Frame_clone(NULL, (ttLibC_Frame *)yuv));
  // なんか処理する。
  r = pthread_mutex_unlock(&decoder->frameMutex_);
  if(r != 0) {
    puts("unlockエラー");
  }
#endif
  return true;
}

bool VtDecompressSessionDecoder::decode(ttLibC_Frame *frame) {
#ifdef __ENABLE_APPLE__
  if(decoder_ == NULL) {
    return false;
  }
  if(frame == NULL) {
    return true;
  }
  if(frame->type != decoder_->frame_type) {
    puts("指定したフレームのみ処理可能です。");
    return false;
  }
  ttLibC_VtDecoder_decode(
    decoder_,
    (ttLibC_Video *)frame,
    decodeCallback,
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
        jsFrame
      };
      Local<Value> jsResult = callbackCall(callback, 1, args);
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
