#include "predef.h"
#include "frame.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ttLibC/allocator.h>
#include "log.h"
#include <ttLibC/frame/audio/audio.h>
#include <ttLibC/frame/audio/aac.h>
#include <ttLibC/frame/audio/adpcmImaWav.h>
#include <ttLibC/frame/audio/mp3.h>
#include <ttLibC/frame/audio/nellymoser.h>
#include <ttLibC/frame/audio/opus.h>
#include <ttLibC/frame/audio/pcmAlaw.h>
#include <ttLibC/frame/audio/pcmf32.h>
#include <ttLibC/frame/audio/pcmMulaw.h>
#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/frame/audio/speex.h>
#include <ttLibC/frame/audio/vorbis.h>
#include <ttLibC/frame/video/video.h>
#include <ttLibC/frame/video/bgr.h>
#include <ttLibC/frame/video/flv1.h>
#include <ttLibC/frame/video/h264.h>
#include <ttLibC/frame/video/h265.h>
#include <ttLibC/frame/video/jpeg.h>
#include <ttLibC/frame/video/theora.h>
#include <ttLibC/frame/video/vp6.h>
#include <ttLibC/frame/video/vp8.h>
#include <ttLibC/frame/video/vp9.h>
#include <ttLibC/frame/video/wmv1.h>
#include <ttLibC/frame/video/wmv2.h>
#include <ttLibC/frame/video/yuv420.h>

#include <ttLibC/util/hexUtil.h>

void TTLIBJSGYP_CDECL Frame::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Frame").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "getBinaryBuffer", GetBinaryBuffer);
  SetPrototypeMethod(tpl, "clone", Clone);
  Local<Function> func = Nan::GetFunction(tpl).ToLocalChecked();
  constructor().Reset(func);
  Nan::Set(func, Nan::New("fromBinaryBuffer").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(FromBinaryBuffer)).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Frame").ToLocalChecked(),
    func);
}

Local<Object> Frame::newInstance() {
  return Nan::NewInstance(
    Nan::New(constructor()),
    0,
    NULL).ToLocalChecked();
}

bool Frame::setFrame(Local<Object> jsFrame, ttLibC_Frame *ttFrame) {
#define SetProperty(target, value) do { \
  Nan::Set(jsFrame, Nan::New(#target).ToLocalChecked(), Nan::New(value)); \
} while(0)
#define SetPropertyChecked(target, value) do { \
  Nan::Set(jsFrame, Nan::New(#target).ToLocalChecked(), Nan::New(value).ToLocalChecked()); \
} while(0)
  // ここでframeの内容を更新したりしないといけない。
  // 内包しているFrameオブジェクトとjsFrameの内容とttFrameの内容の３つが一致していなければならない。
  // この動作はjsFrameの内容を上書きして、内部で保持しているttLibC_Frameを更新することが目的
  /*
  type bgr yuv h264...
  width
  height
  videoType key inner info
  sampleRate
  sampleNum
  channelNum
  data
  lData
  rData
  yData
  uData
  vData
  stride
  yStride
  uStride
  vStride
  subType
  h26xType
  */
  // jsFrameが保持しているid timebase ptsを参照しなければならない。
  // 変更している場合は、保持しているデータが更新する。
  // とりあえずはじめは全部更新でつくっておくか
  SetProperty(pts, (double)ttFrame->pts);
  SetProperty(timebase, ttFrame->timebase);
  SetProperty(id, ttFrame->id);
  SetPropertyChecked(type, "");
  SetPropertyChecked(videoType, "");
  SetPropertyChecked(subType, "");
  SetPropertyChecked(h26xType, "");
  if(ttLibC_Frame_isAudio(ttFrame)) {
    ttLibC_Audio *audio = (ttLibC_Audio *)ttFrame;
    SetProperty(sampleRate, audio->sample_rate);
    SetProperty(sampleNum, audio->sample_num);
    SetProperty(channelNum, audio->channel_num);
    switch(ttFrame->type) {
    case frameType_aac:
      {
        ttLibC_Aac *aac = (ttLibC_Aac *)audio;
        SetPropertyChecked(type, "aac");
        switch(aac->type) {
        case AacType_raw:
          SetPropertyChecked(subType, "raw");
          break;
        case AacType_adts:
          SetPropertyChecked(subType, "adts");
          break;
        case AacType_dsi:
          SetPropertyChecked(subType, "dsi");
          break;
        default:
          break;
        }
      }
      break;
    case frameType_adpcm_ima_wav:
      SetPropertyChecked(type, "adpcmImaWav");
      break;
    case frameType_mp3:
      {
        ttLibC_Mp3 *mp3 = (ttLibC_Mp3 *)audio;
        SetPropertyChecked(type, "mp3");
        switch(mp3->type) {
        case Mp3Type_tag:
          SetPropertyChecked(subType, "tag");
          break;
        case Mp3Type_id3:
          SetPropertyChecked(subType, "id3");
          break;
        case Mp3Type_frame:
          SetPropertyChecked(subType, "frame");
          break;
        default:
          break;
        }
      }
      break;
    case frameType_nellymoser:
      SetPropertyChecked(type, "nellymoser");
      break;
    case frameType_opus:
      {
        ttLibC_Opus *opus = (ttLibC_Opus *)audio;
        SetPropertyChecked(type, "opus");
        switch(opus->type) {
        case OpusType_header:
          SetPropertyChecked(subType, "header");
          break;
        case OpusType_comment:
          SetPropertyChecked(subType, "comment");
          break;
        case OpusType_frame:
          SetPropertyChecked(subType, "frame");
          break;
        default:
          break;
        }
      }
      break;
    case frameType_pcm_alaw:
      SetPropertyChecked(type, "pcmAlaw");
      break;
    case frameType_pcmF32:
      {
        ttLibC_PcmF32 *pcm = (ttLibC_PcmF32 *)audio;
        SetPropertyChecked(type, "pcmF32");
        switch(pcm->type) {
        case PcmF32Type_interleave:
          SetPropertyChecked(subType, "interleave");
          break;
        case PcmF32Type_planar:
          SetPropertyChecked(subType, "planar");
        default:
          break;
        }
        SetProperty(lData, 0);
        SetProperty(lStride, pcm->l_stride);
        SetProperty(rData, 0);
        SetProperty(rStride, pcm->r_stride);
      }
      break;
    case frameType_pcm_mulaw:
      SetPropertyChecked(type, "pcmMulaw");
      break;
    case frameType_pcmS16:
      {
        ttLibC_PcmS16 *pcm = (ttLibC_PcmS16 *)audio;
        SetPropertyChecked(type, "pcmS16");
        switch(pcm->type) {
        case PcmS16Type_bigEndian:
          SetPropertyChecked(subType, "bigEndian");
          break;
        case PcmS16Type_bigEndian_planar:
          SetPropertyChecked(subType, "bigEndianPlanar");
          break;
        case PcmS16Type_littleEndian:
          SetPropertyChecked(subType, "littleEndian");
          break;
        case PcmS16Type_littleEndian_planar:
          SetPropertyChecked(subType, "littleEndianPlanar");
          break;
        default:
          break;
        }
        SetProperty(lData, 0);
        SetProperty(lStride, pcm->l_stride);
        SetProperty(rData, 0);
        SetProperty(rStride, pcm->r_stride);
      }
      break;
    case frameType_speex:
      {
        ttLibC_Speex *speex = (ttLibC_Speex *)audio;
        SetPropertyChecked(type, "speex");
        switch(speex->type) {
        case SpeexType_header:
          SetPropertyChecked(subType, "header");
          break;
        case SpeexType_comment:
          SetPropertyChecked(subType, "comment");
          break;
        case SpeexType_frame:
          SetPropertyChecked(subType, "frame");
          break;
        default:
          break;
        }
      }
      break;
    case frameType_vorbis:
      {
        ttLibC_Vorbis *vorbis = (ttLibC_Vorbis *)audio;
        SetPropertyChecked(type, "vorbis");
        switch(vorbis->type) {
        case VorbisType_identification:
          SetPropertyChecked(subType, "identification");
          break;
        case VorbisType_comment:
          SetPropertyChecked(subType, "comment");
          break;
        case VorbisType_setup:
          SetPropertyChecked(subType, "setup");
          break;
        case VorbisType_frame:
          SetPropertyChecked(subType, "frame");
          break;
        default:
          break;
        }
      }
      break;
    default:
      ERR_PRINT("不明なフレームタイプでした。");
      return false;
    }
  }
  else if(ttLibC_Frame_isVideo(ttFrame)) {
    ttLibC_Video *video = (ttLibC_Video *)ttFrame;
    SetProperty(width, video->width);
    SetProperty(height, video->height);
    switch(video->type) {
    case videoType_key:
      SetPropertyChecked(videoType, "key");
      break;
    case videoType_inner:
      SetPropertyChecked(videoType, "inner");
      break;
    case videoType_info:
      SetPropertyChecked(videoType, "info");
      break;
    default:
      break;
    }
    switch(ttFrame->type) {
    case frameType_bgr:
      {
        ttLibC_Bgr *bgr = (ttLibC_Bgr *)video;
        SetPropertyChecked(type, "bgr");
        switch(bgr->type) {
        case BgrType_abgr:
          SetPropertyChecked(subType, "abgr");
          break;
        case BgrType_bgr:
          SetPropertyChecked(subType, "bgr");
          break;
        case BgrType_bgra:
          SetPropertyChecked(subType, "bgra");
          break;
        default:
          break;
        }
      }
      break;
    case frameType_flv1:
      {
        ttLibC_Flv1 *flv1 = (ttLibC_Flv1 *)video;
        SetPropertyChecked(type, "flv1");
        bool isDisposable = false;
        switch(flv1->type) {
        case Flv1Type_intra:
          SetPropertyChecked(subType, "intra");
          break;
        case Flv1Type_inner:
          SetPropertyChecked(subType, "inner");
          break;
        case Flv1Type_disposableInner:
          SetPropertyChecked(subType, "disposableInner");
          isDisposable = true;
          break;
        default:
          break;
        }
        SetProperty(isDisposable, isDisposable);
      }
      break;
    case frameType_h264:
      {
        ttLibC_H264 *h264 = (ttLibC_H264 *)video;
        SetPropertyChecked(type, "h264");
        switch(h264->type) {
        case H264Type_configData:
          SetPropertyChecked(subType, "configData");
          break;
        case H264Type_sliceIDR:
          SetPropertyChecked(subType, "sliceIDR");
          break;
        case H264Type_slice:
          SetPropertyChecked(subType, "slice");
          break;
        default:
          break;
        }
        switch(h264->frame_type) {
        case H264FrameType_P:
          SetPropertyChecked(h26xType, "P");
          break;
        case H264FrameType_B:
          SetPropertyChecked(h26xType, "B");
          break;
        case H264FrameType_I:
          SetPropertyChecked(h26xType, "I");
          break;
        case H264FrameType_SP:
          SetPropertyChecked(h26xType, "SP");
          break;
        case H264FrameType_SI:
          SetPropertyChecked(h26xType, "SI");
          break;
        default:
          break;
        }
        SetProperty(isDisposable, h264->is_disposable);
      }
      break;
    case frameType_h265:
      {
        ttLibC_H265 *h265 = (ttLibC_H265 *)video;
        SetPropertyChecked(type, "h265");
        switch(h265->type) {
        case H265Type_configData:
          SetPropertyChecked(subType, "configData");
          break;
        case H265Type_sliceIDR:
          SetPropertyChecked(subType, "sliceIDR");
          break;
        case H265Type_slice:
          SetPropertyChecked(subType, "slice");
          break;
        default:
          break;
        }
        switch(h265->frame_type) {
        case H265FrameType_B:
          SetPropertyChecked(h26xType, "B");
          break;
        case H265FrameType_P:
          SetPropertyChecked(h26xType, "P");
          break;
        case H265FrameType_I:
          SetPropertyChecked(h26xType, "I");
          break;
        default:
          break;
        }
        SetProperty(isDisposable, h265->is_disposable);
      }
      break;
    case frameType_jpeg:
      SetPropertyChecked(type, "jpeg");
      break;
    case frameType_theora:
      {
        ttLibC_Theora *theora = (ttLibC_Theora *)video;
        SetPropertyChecked(type, "theora");
        switch(theora->type) {
        case TheoraType_identificationHeaderDecodeFrame:
          SetPropertyChecked(subType, "identification");
          break;
        case TheoraType_commentHeaderFrame:
          SetPropertyChecked(subType, "comment");
          break;
        case TheoraType_setupHeaderFrame:
          SetPropertyChecked(subType, "setup");
          break;
        case TheoraType_intraFrame:
          SetPropertyChecked(subType, "intra");
          break;
        case TheoraType_innerFrame:
          SetPropertyChecked(subType, "inner");
          break;
        default:
          break;
        }
      }
      break;
    case frameType_vp6:
      SetPropertyChecked(type, "vp6");
      break;
    case frameType_vp8:
      SetPropertyChecked(type, "vp8");
      break;
    case frameType_vp9:
      SetPropertyChecked(type, "vp9");
      break;
    case frameType_wmv1:
      SetPropertyChecked(type, "wmv1");
      break;
    case frameType_wmv2:
      SetPropertyChecked(type, "wmv2");
      break;
    case frameType_yuv420:
      {
        ttLibC_Yuv420 *yuv = (ttLibC_Yuv420 *)video;
        SetPropertyChecked(type, "yuv");
        switch(yuv->type) {
        case Yuv420Type_planar:
          SetPropertyChecked(subType, "planar");
          break;
        case Yuv420Type_semiPlanar:
          SetPropertyChecked(subType, "semiPlanar");
          break;
        case Yvu420Type_planar:
          SetPropertyChecked(subType, "yvuPlanar");
          break;
        case Yvu420Type_semiPlanar:
          SetPropertyChecked(subType, "yvuSemiPlanar");
          break;
        default:
          break;
        }
      }
      break;
    default:
      puts("不明なフレームタイプでした。");
      return false;
    }
  }
  else {
    puts("映像でも音声でもないフレームでした。");
    return false;
  }
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  frame->frame_ = ttFrame;
  frame->isRef_ = true;
  // ここでframe->ptr_[0 - 2]のデータについて、リンクをもっておく必要がある。
  switch(frame->frame_->type) {
  case frameType_pcmF32:
    {
      ttLibC_PcmF32 *pcm = (ttLibC_PcmF32 *)frame->frame_;
      frame->ptr_[0] = pcm->l_data;
      frame->ptr_[1] = pcm->r_data;
    }
    break;
  case frameType_pcmS16:
    {
      ttLibC_PcmS16 *pcm = (ttLibC_PcmS16 *)frame->frame_;
      frame->ptr_[0] = pcm->l_data;
      frame->ptr_[1] = pcm->r_data;
    }
    break;
  case frameType_bgr:
    {
      ttLibC_Bgr *bgr = (ttLibC_Bgr *)frame->frame_;
      frame->ptr_[0] = bgr->data;
    }
    break;
  case frameType_yuv420:
    {
      ttLibC_Yuv420 *yuv = (ttLibC_Yuv420 *)frame->frame_;
      frame->ptr_[0] = yuv->y_data;
      frame->ptr_[1] = yuv->u_data;
      frame->ptr_[2] = yuv->v_data;
    }
    break;
  default:
    break;
  }
#undef SetProperty
#undef SetPropertyChecked
  return true;
}

ttLibC_Frame_Type Frame::getFrameType(std::string name) {
  if(name == "aac") {
    return frameType_aac;
  }
  else if(name == "adpcmImaWav") {
    return frameType_adpcm_ima_wav;
  }
  else if(name == "mp3") {
    return frameType_mp3;
  }
  else if(name == "nellymoser") {
    return frameType_nellymoser;
  }
  else if(name == "opus") {
    return frameType_opus;
  }
  else if(name == "pcmAlaw") {
    return frameType_pcm_alaw;
  }
  else if(name == "pcmF32") {
    return frameType_pcmF32;
  }
  else if(name == "pcmMulaw") {
    return frameType_pcm_mulaw;
  }
  else if(name == "pcmS16") {
    return frameType_pcmS16;
  }
  else if(name == "speex") {
    return frameType_speex;
  }
  else if(name == "vorbis") {
    return frameType_vorbis;
  }
  else if(name == "bgr") {
    return frameType_bgr;
  }
  else if(name == "flv1") {
    return frameType_flv1;
  }
  else if(name == "h264") {
    return frameType_h264;
  }
  else if(name == "h265") {
    return frameType_h265;
  }
  else if(name == "jpeg") {
    return frameType_jpeg;
  }
  else if(name == "theora") {
    return frameType_theora;
  }
  else if(name == "vp6") {
    return frameType_vp6;
  }
  else if(name == "vp8") {
    return frameType_vp8;
  }
  else if(name == "vp9") {
    return frameType_vp9;
  }
  else if(name == "wmv1") {
    return frameType_wmv1;
  }
  else if(name == "wmv2") {
    return frameType_wmv2;
  }
  else if(name == "yuv") {
    return frameType_yuv420;
  }
  return frameType_unknown;
}

ttLibC_Frame *Frame::refFrame(Local<Value> jsVFrame) {
#define GetJsFrameInt(target) do { \
  Local<Value> Js##target = Nan::Get(jsFrame, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsNumber() || Js##target->IsUint32() || Js##target->IsInt32()) { \
    target = Js##target->Uint32Value(); \
  } \
} while(0)
#define GetJsFrameLong(target) do { \
  Local<Value> Js##target = Nan::Get(jsFrame, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsNumber() || Js##target->IsUint32() || Js##target->IsInt32()) { \
    target = (uint64_t)Js##target->NumberValue(); \
  } \
} while(0)
// stringは使いにくいな・・・std::stringにいれてから処理する以外方法がないのか・・・
#define GetJsFrameString(target) do { \
  Local<Value> Js##target = Nan::Get(jsFrame, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsString()) { \
    target = std::string(*String::Utf8Value(Js##target->ToString())); \
  } \
} while(0)
  if(!jsVFrame->IsObject()) {
    return NULL;
  }
  if(jsVFrame->IsArrayBuffer() || jsVFrame->IsTypedArray()) {
    return NULL;
  }
  Local<Object> jsFrame = jsVFrame->ToObject();

  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  // 以下の3つはデフォルトで必要なもの。
  uint32_t id       = frame->frame_->id;
  uint64_t pts      = frame->frame_->pts;
  uint32_t timebase = frame->frame_->timebase;
  GetJsFrameInt(id);
  GetJsFrameLong(pts);
  GetJsFrameInt(timebase);
  frame->frame_->id = id;
  frame->frame_->pts = pts;
  frame->frame_->timebase = timebase;
  // まだまだ追加すべきデータがあるはず。strideとかdataPosとか
  // ここの調整を実施すべき。
  // width heightの値をjsFrameのプロパティから復元したりする予定。
  switch(frame->frame_->type) {
//  case frameType_aac:
  case frameType_adpcm_ima_wav:
//  case frameType_mp3:
  case frameType_nellymoser:
//  case frameType_opus:
  case frameType_pcm_alaw:
  case frameType_pcmF32:
  case frameType_pcm_mulaw:
  case frameType_pcmS16:
//  case frameType_speex:
//  case frameType_vorbis:
    {
      ttLibC_Audio *audio = (ttLibC_Audio *)frame->frame_;
      uint32_t sampleRate = audio->sample_rate;
      uint32_t sampleNum  = audio->sample_num;
      uint32_t channelNum = audio->channel_num;
      GetJsFrameInt(sampleRate);
      GetJsFrameInt(sampleNum);
      GetJsFrameInt(channelNum);
      audio->sample_rate = sampleRate;
      audio->sample_num  = sampleNum;
      audio->channel_num = channelNum;
      switch(frame->frame_->type) {
      case frameType_pcmF32:
        {
          ttLibC_PcmF32 *pcm = (ttLibC_PcmF32 *)audio;
          uint32_t rData = 0;
          uint32_t lData = 0;
          GetJsFrameInt(rData); // あくまでズレはbyte数としておく。
          GetJsFrameInt(lData);
          pcm->l_data = frame->ptr_[0] + lData;
          pcm->r_data = frame->ptr_[1] + rData;
          // strideも更新すべきか？(とりあえず使わないと思うので、しばし放置しておく)
        }
        break;
      case frameType_pcmS16:
        {
          ttLibC_PcmS16 *pcm = (ttLibC_PcmS16 *)audio;
          uint32_t rData = 0;
          uint32_t lData = 0;
          GetJsFrameInt(rData); // あくまでズレはbyte数としておく。
          GetJsFrameInt(lData);
          pcm->l_data = frame->ptr_[0] + lData;
          pcm->r_data = frame->ptr_[1] + rData;
          // strideも更新すべきか？(とりあえず使わないと思うので、しばし放置しておく)
        }
        break;
      default:
        break;
      }
    }
    break;

  case frameType_bgr:
//  case frameType_flv1:
//  case frameType_h264:
//  case frameType_h265:
//  case frameType_jpeg:
//  case frameType_theora:
  case frameType_vp6:
//  case frameType_vp8:
//  case frameType_vp9:
  case frameType_wmv1:
  case frameType_wmv2:
  case frameType_yuv420:
    {
      ttLibC_Video *video = (ttLibC_Video *)frame->frame_;
      uint32_t width  = video->width;
      uint32_t height = video->height;
      std::string videoType("");
      GetJsFrameInt(width);
      GetJsFrameInt(height);
      GetJsFrameString(videoType);
      video->width  = width;
      video->height = height;
      if(videoType == "key") {
        video->type = videoType_key;
      }
      else if(videoType == "inner") {
        video->type = videoType_inner;
      }
      else if(videoType == "info") {
        video->type = videoType_info;
      }
      // bgr、yuvの場合はrefPointerの開始位置を変更したり、stride値を書き換えたりするかもしれない(strideは普通は変更しないか・・・)
      switch(frame->frame_->type) {
      case frameType_bgr:
        {
          ttLibC_Bgr *bgr = (ttLibC_Bgr *)video;
          uint32_t data   = 0;
          uint32_t stride = bgr->width_stride; // オリジナルの値に戻してやった方が幸せかね。
          GetJsFrameInt(data);
          GetJsFrameInt(stride);
          bgr->data = frame->ptr_[0] + data;
          bgr->width_stride = stride;
        }
        break;
      case frameType_yuv420:
        {
          ttLibC_Yuv420 *yuv = (ttLibC_Yuv420 *)video;
          uint32_t yData = 0;
          uint32_t uData = 0;
          uint32_t vData = 0;
          uint32_t yStride = yuv->y_stride;
          uint32_t uStride = yuv->u_stride;
          uint32_t vStride = yuv->v_stride;
          GetJsFrameInt(yData);
          GetJsFrameInt(uData);
          GetJsFrameInt(vData);
          GetJsFrameInt(yStride);
          GetJsFrameInt(uStride);
          GetJsFrameInt(vStride);
          yuv->y_data = frame->ptr_[0] + yData;
          yuv->u_data = frame->ptr_[1] + uData;
          yuv->v_data = frame->ptr_[2] + vData;
          yuv->y_stride = yStride;
          yuv->u_stride = uStride;
          yuv->v_stride = vStride;
        }
        break;
      default:
        break;
      }
    }
    break;
  default:
    break;
  }
#undef GetJsFrameInt
#undef GetJsFrameLong
#undef GetJsFrameString
  return frame->frame_;
}

NAN_METHOD(Frame::New) {
  /*
  何もないところからつくって　-> ttLibC_Frameを追加して扱う
  JsのFrameオブジェクトから、内部のttLibC_Frameのcloneをつくって、それを利用するものを作っておいたら幸せになれそうではある。
  コンストラクタではなくて、cloneというメソッドを準備しておく方がいいかな。
  BinaryデータからFrameオブジェクトを復元する動作もほしい。
  これもFrameを空から再生成して・・・とした方がいいかね。
   */
  if(info.IsConstructCall()) {
    Frame *frame = new Frame();
    frame->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }
  else {
    info.GetReturnValue().Set(newInstance());
  }
}

NAN_METHOD(Frame::GetBinaryBuffer) {
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(info.Holder());
  if(frame->frame_ == NULL) {
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  info.GetReturnValue().Set(Nan::CopyBuffer((char *)frame->frame_->data, frame->frame_->buffer_size).ToLocalChecked());
}

/**
 * binaryStringからJsFrameを復元します
 * @param prevFrame 前回つかった同じ系列のデータ
 * @param buffer    frameにしたいbinaryデータ
 * @param params    その他追加データ 連想配列で設定
 * @return Frameデータ、失敗時にはnullが応答される。
 *
 *  paramsの内容
 *   type: フレームのタイプを文字列で指定
 *   id: フレームのIDを数値で指定
 *   pts: フレームのptsを数値で指定
 *   timebase: フレームのtimebaseを数値で指定
 *   ここまでが基本
 *   subType: pcmS16、pcmF32、bgr、yuvの場合の詳細設定をいれる。
 *   width: 横幅
 *   height: 縦幅
 *   stride: bgrのstride値
 *   yStride: yuvのy要素のstride値
 *   uStride: yuvのu要素のstride値
 *   vStride: yuvのv要素のstride値
 *   sampleRate: サンプルレート pcmS16 pcmF32 nellymoser pcmAlaw pcmMulaw adpcmImaWav等で利用する
 *   channelNum: チャンネル数 pcmS16 pcmF32 nellymoser pcmAlaw pcmMulaw adpcmImaWav等で利用する
 *   sampleNum: サンプル数 pcmS16 pcmF32 nellymoser pcmAlaw pcmMulaw adpcmImaWav等で利用する
 *   adjustment: vp6のadjustmentデータ
 *   videoType: wmv1 wmv2で利用するkey inner infoの指定
 * といった具合
 */
NAN_METHOD(Frame::FromBinaryBuffer) {
#define GetParamInt(target) do { \
  Local<Value> Js##target = Nan::Get(params, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsNumber() || Js##target->IsUint32() || Js##target->IsInt32()) { \
    target = Js##target->Uint32Value(); \
  } \
} while(0)
#define GetParamLong(target) do { \
  Local<Value> Js##target = Nan::Get(params, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsNumber() || Js##target->IsUint32() || Js##target->IsInt32()) { \
    target = (uint64_t)Js##target->NumberValue(); \
  } \
} while(0)
#define GetParamString(target) do { \
  Local<Value> Js##target = Nan::Get(params, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsString()) { \
    target = std::string(*String::Utf8Value(Js##target->ToString())); \
  } \
} while(0)
  // binaryBuffer(ArrayBufferやTypedDataView、Buffer)からttLibC_Frameを復元する動作
  // 基本Cloneと同じ
  // prevFrame, buffer, paramsとしよう。
  // paramsの中にtype, id, pts, timebaseの基本情報や
  // prevFrameの内容を継承できるものは、そっちを優先するということで・・・
  // y_strideとかsubTypeとかいろいろいれるべき
  if(info.Length() < 2) {
    // info.Lengthが2未満の場合は動作不能。
    ERR_PRINT("入力パラメーターが足りません");
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  ttLibC_Frame *prevFrame = refFrame(info[0]);
  Local<Object> params;
  std::string type("");
  ttLibC_Frame_Type frameType;
  uint8_t *data;
  size_t data_size;
  uint64_t pts = 0;
  uint32_t timebase = 1000;
  uint32_t id = 0;
  if(!info[1]->IsTypedArray() && !info[1]->IsArrayBuffer()) {
    ERR_PRINT("fromBinaryBufferの入力データが不正です。");
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  // とりあえずデフォルトの動作として、prevFrameの内容を踏襲する。
  // 可能な限りではあるが・・・
  if(prevFrame != NULL) {
    frameType = prevFrame->type;
    id        = prevFrame->id;
    timebase  = prevFrame->timebase;
  }
  data = (uint8_t *)node::Buffer::Data(info[1]->ToObject());
  data_size = node::Buffer::Length(info[1]->ToObject());
  if(info.Length() <= 3 && info[2]->IsObject()) {
    // 2つ目のパラメーターが連想配列であるとして処理しておく
    params = info[2]->ToObject();
  }
  else {
    params = Nan::New<Object>();
  }
  GetParamString(type);
  if(type != "") {
    // frameTypeを更新
    frameType = getFrameType(type);
  }
  // とりあえず基本
  GetParamInt(id);
  GetParamLong(pts);
  GetParamInt(timebase);
  ttLibC_Frame *frame = NULL;
  switch(frameType) {
  case frameType_aac:
    {
      frame = (ttLibC_Frame *)ttLibC_Aac_getFrame(
          (ttLibC_Aac *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_adpcm_ima_wav:
    {
      uint32_t sampleRate = 0;
      uint32_t sampleNum = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      GetParamInt(sampleNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。adpcm_ima_wavを復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      frame = (ttLibC_Frame *)ttLibC_AdpcmImaWav_make(
          (ttLibC_AdpcmImaWav *)prevFrame,
          sampleRate,
          sampleNum,
          channelNum,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_mp3:
    {
      frame = (ttLibC_Frame *)ttLibC_Mp3_getFrame(
          (ttLibC_Mp3 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_nellymoser:
    {
      uint32_t sampleRate = 0;
      uint32_t sampleNum = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。nellymoser復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      GetParamInt(sampleNum);
      // こっちはsampleRateはどうなるか不明だけど、仮として44100をいれておく。
      // channelは1
      // あとでjsFrameの属性を変更したら、それを反映するようにすればよい。
      frame = (ttLibC_Frame *)ttLibC_Nellymoser_make(
          (ttLibC_Nellymoser *)prevFrame,
          sampleRate,
          sampleNum,
          channelNum,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_opus:
    {
      frame = (ttLibC_Frame *)ttLibC_Opus_getFrame(
          (ttLibC_Opus *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_pcm_alaw:
    {
      uint32_t sampleRate = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。pcmAlaw復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      // 仮でsampleRate = 16000、channelNum = 1で設定して構築しておく。
      frame = (ttLibC_Frame *)ttLibC_PcmAlaw_make(
          (ttLibC_PcmAlaw *)prevFrame,
          sampleRate,
          data_size,
          channelNum,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_pcmF32:
    {
      std::string subType = "interleave";
      uint32_t sampleRate = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。pcmF32復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      uint32_t sampleNum = data_size / channelNum / sizeof(float);
      GetParamInt(sampleNum);
      ttLibC_PcmF32_Type frameSubType = PcmF32Type_interleave;
      uint8_t *newData = (uint8_t *)ttLibC_malloc(data_size);
      if(newData == NULL) {
        ERR_PRINT("memory alloc失敗しました。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      memcpy(newData, data, data_size);
      uint8_t *lData = newData;
      uint8_t *rData = NULL;
      size_t lStride = data_size;
      size_t rStride = 0;
      if(subType == "interleave") {
        frameSubType = PcmF32Type_interleave;
        lStride = sampleNum * 4 * channelNum;
        GetParamInt(lStride);
        rData = NULL;
        rStride = 0;
      }
      else if(subType == "planar") {
        frameSubType = PcmF32Type_planar;
        lStride = sampleNum * 4;
        GetParamInt(lStride);
        if(channelNum == 2) {
          rData = lData + lStride;
          rStride = lStride;
          GetParamInt(rStride);
        }
      }
      frame = (ttLibC_Frame *)ttLibC_PcmF32_make(
          (ttLibC_PcmF32 *)prevFrame,
          frameSubType,
          sampleRate,
          sampleNum,
          channelNum,
          newData,
          data_size,
          lData,
          lStride,
          rData,
          rStride,
          true,
          pts,
          timebase);
      if(frame != NULL) {
        frame->is_non_copy = false;
      }
    }
    break;
  case frameType_pcm_mulaw:
    {
      uint32_t sampleRate = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。pcmMulaw復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      // こっちも仮でsampleRate = 16000、channelNum = 1で設定する。
      frame = (ttLibC_Frame *)ttLibC_PcmMulaw_make(
          (ttLibC_PcmMulaw *)prevFrame,
          sampleRate,
          data_size,
          channelNum,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_pcmS16:
    {
      std::string subType = "littleEndian";
      uint32_t sampleRate = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。pcmS16復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      uint32_t sampleNum = data_size / channelNum / sizeof(int16_t);
      GetParamInt(sampleNum);
      ttLibC_PcmS16_Type frameSubType = PcmS16Type_littleEndian;
      uint8_t *newData = (uint8_t *)ttLibC_malloc(data_size);
      if(newData == NULL) {
        ERR_PRINT("memory alloc失敗しました。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      memcpy(newData, data, data_size);
      uint8_t *lData = newData;
      uint8_t *rData = NULL;
      size_t lStride = data_size;
      size_t rStride = 0;
      if(subType == "littleEndian") {
        frameSubType = PcmS16Type_littleEndian;
        lStride = sampleNum * 2 * channelNum;
        GetParamInt(lStride);
        rData = NULL;
        rStride = 0;
      }
      else if(subType == "littleEndianPlanar") {
        frameSubType = PcmS16Type_littleEndian_planar;
        lStride = sampleNum * 2;
        GetParamInt(lStride);
        if(channelNum == 2) {
          rData = lData + lStride;
          rStride = lStride;
          GetParamInt(rStride);
        }
      }
      else if(subType == "bigEndian") {
        frameSubType = PcmS16Type_bigEndian;
        lStride = sampleNum * 2 * channelNum;
        GetParamInt(lStride);
        rData = NULL;
        rStride = 0;
      }
      else if(subType == "bigEndianPlanar") {
        frameSubType = PcmS16Type_bigEndian_planar;
        lStride = sampleNum * 2;
        GetParamInt(lStride);
        if(channelNum == 2) {
          rData = lData + lStride;
          rStride = lStride;
          GetParamInt(rStride);
        }
      }
      frame = (ttLibC_Frame *)ttLibC_PcmS16_make(
          (ttLibC_PcmS16 *)prevFrame,
          frameSubType,
          sampleRate,
          sampleNum,
          channelNum,
          newData,
          data_size,
          lData,
          lStride,
          rData,
          rStride,
          true,
          pts,
          timebase);
      if(frame != NULL) {
        frame->is_non_copy = false;
      }
    }
    break;
  case frameType_speex:
    {
      frame = (ttLibC_Frame *)ttLibC_Speex_getFrame(
          (ttLibC_Speex *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_vorbis:
    {
      frame = (ttLibC_Frame *)ttLibC_Vorbis_getFrame(
          (ttLibC_Vorbis *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;

  case frameType_bgr:
    {
      std::string subType = "bgr";
      uint32_t width  = 0;
      uint32_t height = 0;
      GetParamInt(width);
      GetParamInt(height);
      if(width == 0 || height == 0) {
        ERR_PRINT("widthまたはheightが未設定です。bgr復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      uint32_t stride = width * 3;
      GetParamString(subType);
      ttLibC_Bgr_Type frameSubType = BgrType_bgr;
      if(subType == "bgr") {
        GetParamInt(stride);
        frameSubType = BgrType_bgr;
      }
      else if(subType == "bgra") {
        stride = width * 4;
        GetParamInt(stride);
        frameSubType = BgrType_bgra;
      }
      else if(subType == "abgr") {
        stride = width * 4;
        GetParamInt(stride);
        frameSubType = BgrType_abgr;
      }
      frame = (ttLibC_Frame *)ttLibC_Bgr_make(
          (ttLibC_Bgr *)prevFrame,
          frameSubType,
          width,
          height,
          stride,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_flv1:
    {
      frame = (ttLibC_Frame *)ttLibC_Flv1_getFrame(
          (ttLibC_Flv1 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_h264:
    {
      frame = (ttLibC_Frame *)ttLibC_H264_getFrame(
          (ttLibC_H264 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_h265:
    {
      frame = (ttLibC_Frame *)ttLibC_H265_getFrame(
          (ttLibC_H265 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_jpeg:
    {
      frame = (ttLibC_Frame *)ttLibC_Jpeg_getFrame(
          (ttLibC_Jpeg *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_theora:
    {
      frame = (ttLibC_Frame *)ttLibC_Theora_getFrame(
          (ttLibC_Theora *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_vp6:
    {
      // adjustmentはいれないけど、あとでframeのwidthを設定すれば、そっちを採用するという形で攻めればいいと思う
      uint32_t adjustment = 0x00;
      GetParamInt(adjustment);
      frame = (ttLibC_Frame *)ttLibC_Vp6_getFrame(
          (ttLibC_Vp6 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase,
          adjustment);
    }
    break;
  case frameType_vp8:
    {
      frame = (ttLibC_Frame *)ttLibC_Vp8_getFrame(
          (ttLibC_Vp8 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_vp9:
    {
      frame = (ttLibC_Frame *)ttLibC_Vp9_getFrame(
          (ttLibC_Vp9 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_wmv1:
    {
      uint32_t width  = 0;
      uint32_t height = 0;
      std::string videoType = "key";
      GetParamInt(width);
      GetParamInt(height);
      if(width == 0 || height == 0) {
        ERR_PRINT("widthまたはheightが未設定です。wmv1復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      GetParamString(videoType);
      ttLibC_Video_Type video_type = videoType_key;
      if(videoType == "key") {
        video_type = videoType_key;
      }
      else if(videoType == "inner") {
        video_type = videoType_inner;
      }
      else if(videoType == "info") {
        video_type = videoType_info;
      }
      frame = (ttLibC_Frame *)ttLibC_Wmv1_make(
          (ttLibC_Wmv1 *)prevFrame,
          video_type,
          width,
          height,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_wmv2:
    {
      uint32_t width  = 0;
      uint32_t height = 0;
      std::string videoType = "key";
      GetParamInt(width);
      GetParamInt(height);
      if(width == 0 || height == 0) {
        ERR_PRINT("widthまたはheightが未設定です。wmv2復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      GetParamString(videoType);
      ttLibC_Video_Type video_type = videoType_key;
      if(videoType == "key") {
        video_type = videoType_key;
      }
      else if(videoType == "inner") {
        video_type = videoType_inner;
      }
      else if(videoType == "info") {
        video_type = videoType_info;
      }
      frame = (ttLibC_Frame *)ttLibC_Wmv2_make(
          (ttLibC_Wmv2 *)prevFrame,
          video_type,
          width,
          height,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_yuv420:
    {
      std::string subType = "planar";
      uint32_t width  = 0;
      uint32_t height = 0;
      GetParamInt(width);
      GetParamInt(height);
      if(width == 0 || height == 0) {
        ERR_PRINT("widthまたはheightが未設定です。yuv420復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      uint32_t yStride = width;
      uint32_t uStride = (width >> 1);
      uint32_t vStride = (width >> 1);

      GetParamString(subType);
      ttLibC_Yuv420_Type frameSubType = Yuv420Type_planar;
      uint8_t *newData = (uint8_t *)ttLibC_malloc(data_size);
      if(newData == NULL) {
        ERR_PRINT("memory alloc失敗しました。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      memcpy(newData, data, data_size);
      uint8_t *yData = newData;
      uint8_t *uData = newData;
      uint8_t *vData = newData;
      if(subType == "planar") {
        GetParamInt(yStride);
        GetParamInt(uStride);
        GetParamInt(vStride);
        uData = yData + height * yStride;
        vData = uData + (height >> 1) * uStride;
      }
      else if(subType == "semiPlanar") {
        uStride = width;
        vStride = width;
        GetParamInt(yStride);
        GetParamInt(uStride);
        GetParamInt(vStride);
        frameSubType = Yuv420Type_semiPlanar;
        uData = yData + height * yStride;
        vData = uData + 1;
      }
      else if(subType == "yvuPlanar") {
        GetParamInt(yStride);
        GetParamInt(uStride);
        GetParamInt(vStride);
        frameSubType = Yvu420Type_planar;
        vData = yData + height * yStride;
        uData = vData + (height >> 1) * vStride;
      }
      else if(subType == "yvuSemiPlanar") {
        uStride = width;
        vStride = width;
        GetParamInt(yStride);
        GetParamInt(uStride);
        GetParamInt(vStride);
        frameSubType = Yvu420Type_semiPlanar;
        vData = yData + height * yStride;
        uData = vData + 1;
      }
      else {
        ERR_PRINT("subTypeが不正です。yuv420復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      frame = (ttLibC_Frame *)ttLibC_Yuv420_make(
          (ttLibC_Yuv420 *)prevFrame,
          frameSubType,
          width,
          height,
          newData,
          data_size,
          yData,
          yStride,
          uData,
          uStride,
          vData,
          vStride,
          true,
          pts,
          timebase);
      if(frame != NULL) {
        frame->is_non_copy = false;
      }
    }
    break;
  default:
    break;
  }
  if(frame == NULL) {
    ERR_PRINT("frameが生成されませんでした。");
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  frame->id = id;
  Local<Object> jsFrame;
  if(prevFrame == NULL) {
    jsFrame = Frame::newInstance();
  }
  else {
    jsFrame = info[0]->ToObject();
  }
  Frame::setFrame(jsFrame, frame);
  Frame *newFrame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  newFrame->isRef_ = false;
  info.GetReturnValue().Set(jsFrame);
#undef GetParamInt
#undef GetParamLong
#undef GetParamString
}

NAN_METHOD(Frame::Clone) {
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(info.Holder());
  Local<Object> jsFrame = Frame::newInstance();
  Frame::setFrame(jsFrame, ttLibC_Frame_clone(NULL, frame->frame_));
  Frame *newFrame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  newFrame->isRef_ = false;
  info.GetReturnValue().Set(jsFrame);
}

Frame::Frame() {
  frame_ = NULL;
  isRef_ = false;
  ptr_[0] = NULL;
  ptr_[1] = NULL;
  ptr_[2] = NULL;
}

Frame::~Frame() {
  if(!isRef_) {
    ttLibC_Frame_close(&frame_);
  }
}
