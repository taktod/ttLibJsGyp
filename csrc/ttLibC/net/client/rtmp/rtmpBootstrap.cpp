#include "rtmpBootstrap.h"

#include "../../../../predef.h"
#include "../../../../frame.h"

#include "tetty/rtmpClientHandler.h"
#include "tetty/rtmpCommandHandler.h"
#include <ttLibC/allocator.h>
#include <ttLibC/net/client/rtmp/tetty/rtmpDecoder.h>
#include <ttLibC/net/client/rtmp/tetty/rtmpEncoder.h>
#include <ttLibC/net/client/rtmp/tetty/rtmpHandshake.h>

#include <ttLibC/net/client/rtmp/message/userControlMessage.h>
#include <ttLibC/net/client/rtmp/message/videoMessage.h>
#include <ttLibC/net/client/rtmp/message/audioMessage.h>

#include <ttLibC/frame/video/h264.h>
#include <ttLibC/frame/audio/aac.h>

#include <ttLibC/util/amfUtil.h>

void TTLIBJSGYP_CDECL RtmpBootstrap::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("RtmpBootstrap").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "connect", Connect);
  SetPrototypeMethod(tpl, "data", Data);
  SetPrototypeMethod(tpl, "createStream", CreateStream);
  SetPrototypeMethod(tpl, "play", Play);
  SetPrototypeMethod(tpl, "publish", Publish);
  SetPrototypeMethod(tpl, "setBufferLength", SetBufferLength);
  SetPrototypeMethod(tpl, "queueFrame", QueueFrame);
  SetPrototypeMethod(tpl, "closeStream", CloseStream);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("RtmpBootstrap").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

ttLibC_FlvFrameManager *RtmpBootstrap::refFrameManager(uint32_t streamId) {
  ttLibC_FlvFrameManager *manager = (ttLibC_FlvFrameManager *)ttLibC_StlMap_get(streamIdFlvFrameManagerMap_, (void *)(long)streamId);
  return manager;
}

void RtmpBootstrap::recvAmf0Command(ttLibC_Amf0Command *command) {
  std::string commandName((const char *)command->command_name);
  if(commandName == "onStatus") {
    // statusメッセージ
    // この該当IDに対してメッセージを発行します。
    // このコマンドのobj2に対してinfoを構築してcallbackすれば良さそう。
    Local<Object> socket = Nan::New(socket_);
    Local<Value> func = Nan::Get(socket, Nan::New("tettyEventCallback").ToLocalChecked()).ToLocalChecked();
    Nan::Callback callback(func.As<Function>());

    Local<Object> jsObject = Nan::New<Object>();
    Nan::Set(jsObject, Nan::New("info").ToLocalChecked(), toJsObject(command->obj2));
    Local<Value> args[] = {
      Nan::New(command->inherit_super.header->stream_id),
      jsObject
    };
    callback.Call(2, args);
  }
  else {
    // onStatus以外のイベント
    puts(commandName.c_str());
  }
}

void RtmpBootstrap::recvAmf0Result(
    ttLibC_Amf0Command *command,
    ttLibC_Amf0Command *result) {
  std::string target_command((const char *)command->command_name);
  if(target_command == "connect") {
    // 接続の応答の場合は、接続のcallbackを処理しなければならない。
    Local<Object> socket = Nan::New(socket_);
    Local<Value> func = Nan::Get(socket, Nan::New("tettyEventCallback").ToLocalChecked()).ToLocalChecked();
    Nan::Callback callback(func.As<Function>());

    Local<Object> jsObject = Nan::New<Object>();
    Nan::Set(jsObject, Nan::New("info").ToLocalChecked(), toJsObject(result->obj2));
    Local<Value> args[] = {
      Nan::New(-1),
      jsObject
    };
    callback.Call(2, args);
  }
  else if(target_command == "createStream") {
    // createStreamを発動した場合の応答動作
    // 取得したstreamIdをcallbackでうまく応答しなければいけないわけだが・・・どうするかね。promiseつかった方がやりやすいかな
    Nan::Callback *callback = (Nan::Callback *)command->promise;
    Local<Value> args[] = {
      Nan::New((uint32_t)(*((double *)result->obj2->object)))
    };
    callback->Call(1, args);
    delete callback;
  }
  else {
    puts((const char *)command->command_name);
  }
}

bool RtmpBootstrap::playFrameCallback(uint32_t streamId, ttLibC_Frame *frame) {
  Local<Object> socket = Nan::New(socket_);
  Local<Value> func = Nan::Get(socket, Nan::New("tettyEventCallback").ToLocalChecked()).ToLocalChecked();
  Nan::Callback callback(func.As<Function>());

  Local<Object> jsFrame = Frame::newInstance();
  Frame::setFrame(jsFrame, frame);
  Local<Value> args[] = {
    Nan::New(streamId),
    jsFrame
  };
  callback.Call(2, args);
  return true;
}

void RtmpBootstrap::sendConnect() {
  ttLibC_Amf0Command *connect = ttLibC_Amf0Command_connect(address_.c_str(), app_.c_str());
  ttLibC_TettyBootstrap_channels_write(bootstrap_, connect, sizeof(ttLibC_Amf0Command));
  ttLibC_TettyBootstrap_channels_flush(bootstrap_);
  ttLibC_Amf0Command_close(&connect);
}

NAN_METHOD(RtmpBootstrap::New) {
  if(info.IsConstructCall()) {
    RtmpBootstrap *bootstrap = new RtmpBootstrap(info[0], info[1]);
    bootstrap->Wrap(info.This());
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

NAN_METHOD(RtmpBootstrap::Connect) {
  // ここでコネクトを実施するわけだが・・・
  // bootstrapのconnectの残り処理をすればいいと思われる。
  // ここからRtmpBootstrapを復元してconnectを実施しないと・・・
  RtmpBootstrap *bootstrap = Nan::ObjectWrap::Unwrap<RtmpBootstrap>(info.Holder());
  if(bootstrap == NULL) {
    info.GetReturnValue().Set(false);
    return;
  }
  bootstrap->socket_.Reset(info[0]->ToObject());
  // ここでソケットオブジェクトを設定しなければならないわけだが・・・
  ttLibC_TettyBootstrap_connectNode(bootstrap->bootstrap_, info[0]->ToObject());
  info.GetReturnValue().Set(true);
}

NAN_METHOD(RtmpBootstrap::Data) {
  RtmpBootstrap *bootstrap = Nan::ObjectWrap::Unwrap<RtmpBootstrap>(info.Holder());
  if(bootstrap == NULL) {
    info.GetReturnValue().Set(false);
    return;
  }
  void  *data      = (void *)node::Buffer::Data(info[1]->ToObject());
  size_t data_size = node::Buffer::Length(info[1]->ToObject());
  ttLibC_TettyBootstrap_fireChannelReadNode(
    bootstrap->bootstrap_,
    info[0]->ToObject(),
    data,
    data_size);
  info.GetReturnValue().Set(true);
}

NAN_METHOD(RtmpBootstrap::CreateStream) {
  // createStreamを実施する。
  // 完了したときにinfo[0]にはいっているcallbackをキックする。
  RtmpBootstrap *bootstrap = Nan::ObjectWrap::Unwrap<RtmpBootstrap>(info.Holder());
  if(bootstrap == NULL) {
    info.GetReturnValue().Set(false);
  }
  // ここでcreateStreamを作らなければならない。
  ttLibC_Amf0Command *createStream = ttLibC_Amf0Command_createStream();
  // ちょっと反則だけど、promiseにinfoのポインタいれとくか・・・
  createStream->promise = (ttLibC_TettyPromise *)(new Nan::Callback(info[0].As<Function>()));
  ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, createStream, sizeof(ttLibC_Amf0Command));
  ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
  ttLibC_Amf0Command_close(&createStream);
}

NAN_METHOD(RtmpBootstrap::Play) {
  RtmpBootstrap *bootstrap = Nan::ObjectWrap::Unwrap<RtmpBootstrap>(info.Holder());
  if(bootstrap == NULL) {
    info.GetReturnValue().Set(false);
  }
  if(info.Length() != 4) {
    info.GetReturnValue().Set(false);
  }
  uint32_t streamId = info[0]->Uint32Value();
  std::string name(*String::Utf8Value(info[1]->ToString()));
  bool acceptVideo = info[2]->IsTrue();
  bool acceptAudio = info[3]->IsTrue();
  // receiveAudioを送る
	ttLibC_Amf0Command *receiveAudio = ttLibC_Amf0Command_receiveAudio(streamId, acceptAudio);
	ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, receiveAudio, sizeof(ttLibC_Amf0Command));
	ttLibC_Amf0Command_close(&receiveAudio);
  // receiveVideoを送る
	ttLibC_Amf0Command *receiveVideo = ttLibC_Amf0Command_receiveVideo(streamId, acceptVideo);
	ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, receiveVideo, sizeof(ttLibC_Amf0Command));
	ttLibC_Amf0Command_close(&receiveVideo);
  // playを送る
	ttLibC_Amf0Command *play = ttLibC_Amf0Command_play(streamId, name.c_str());
	ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, play, sizeof(ttLibC_Amf0Command));
	ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
	ttLibC_Amf0Command_close(&play);
  // playする側のみ、flvFrameManagerが必要なので、つくっておくことにする。
  ttLibC_FlvFrameManager *frameManager = ttLibC_FlvFrameManager_make();
  ttLibC_StlMap_put(bootstrap->streamIdFlvFrameManagerMap_, (void *)(long)streamId, frameManager);
  info.GetReturnValue().Set(true);
}

NAN_METHOD(RtmpBootstrap::Publish) {
  RtmpBootstrap *bootstrap = Nan::ObjectWrap::Unwrap<RtmpBootstrap>(info.Holder());
  if(bootstrap == NULL) {
    info.GetReturnValue().Set(false);
  }
  if(info.Length() != 2) {
    info.GetReturnValue().Set(false);
  }
  uint32_t streamId = info[0]->Uint32Value();
  std::string name(*String::Utf8Value(info[1]->ToString()));
  ttLibC_Amf0Command *publish = ttLibC_Amf0Command_publish(streamId, name.c_str());
  ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, publish, sizeof(ttLibC_Amf0Command));
  ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
  ttLibC_Amf0Command_close(&publish);
  info.GetReturnValue().Set(true);
}

NAN_METHOD(RtmpBootstrap::SetBufferLength) {
  RtmpBootstrap *bootstrap = Nan::ObjectWrap::Unwrap<RtmpBootstrap>(info.Holder());
  if(bootstrap == NULL) {
    info.GetReturnValue().Set(false);
  }
  if(info.Length() != 2) {
    info.GetReturnValue().Set(false);
  }
  ttLibC_UserControlMessage *clientBufferLength = ttLibC_UserControlMessage_make(
      Type_ClientBufferLength,
      info[0]->Uint32Value(),
      info[1]->Uint32Value() * 1000,
      0); // ここにstreamのptsをいれないといけないが・・・とりあえず0いれとくか
  ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, clientBufferLength, sizeof(ttLibC_UserControlMessage));
  ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
  ttLibC_UserControlMessage_close(&clientBufferLength);
  info.GetReturnValue().Set(true);
}

NAN_METHOD(RtmpBootstrap::QueueFrame) {
  RtmpBootstrap *bootstrap = Nan::ObjectWrap::Unwrap<RtmpBootstrap>(info.Holder());
  if(bootstrap == NULL) {
    info.GetReturnValue().Set(false);
  }
  if(info.Length() != 2) {
    info.GetReturnValue().Set(false);
  }
  uint32_t streamId = info[0]->Uint32Value();
  ttg_frameGroup *group = (ttg_frameGroup *)ttLibC_StlMap_get(bootstrap->frameGroupMap_, (void *)(long)streamId);
  if(group == NULL) {
    // groupがない場合(新規作成の場合)
    group = (ttg_frameGroup *)ttLibC_malloc(sizeof(ttg_frameGroup));
    if(group == NULL) {
      // group作成失敗、fatal
      info.GetReturnValue().Set(false);
      return;
    }
    group->audioType = frameType_unknown;
    group->videoType = frameType_unknown;
    group->audioQueue = ttLibC_FrameQueue_make(8, 1024);
    group->videoQueue = ttLibC_FrameQueue_make(9, 1024);
    group->videoQueue->isBframe_fixed = true;
    ttLibC_StlMap_put(bootstrap->frameGroupMap_, (void *)(long)streamId, group);
  }
  // ここまできたら、groupがあるはず。
  ttLibC_Frame *frame = Frame::refFrame(info[1]);

  // pts timebaseを1000に変更しておく必要がありそう
  frame->pts = (uint64_t)(1.0 * frame->pts * 1000 / frame->timebase);
  frame->timebase = 1000;

  switch(frame->type) {
  case frameType_h264:
    {
      ttLibC_H264 *h264 = (ttLibC_H264 *)frame;
      if(h264->type == H264Type_unknown) {
        ERR_PRINT("unknown type of h264 frame.");
        info.GetReturnValue().Set(false);
        return;
      }
      if(h264->type == H264Type_configData) {
        frame->pts = 0;
        frame->dts = 0;
      }
    }
    /* no break */
  case frameType_flv1:
  case frameType_vp6:
    if(group->videoType == frameType_unknown) {
      // 設定がない場合はvideoTypeを設定してもっておく
      group->videoType = frame->type;
    }
    if(group->videoType != frame->type) {
      info.GetReturnValue().Set(false);
      return;
    }
    if(!ttLibC_FrameQueue_queue(group->videoQueue, frame)) {
      info.GetReturnValue().Set(false);
      return;
    }
    break;
  case frameType_aac:
  case frameType_mp3:
  case frameType_nellymoser:
  case frameType_pcm_alaw:
  case frameType_pcm_mulaw:
  case frameType_pcmS16:
  case frameType_speex:
    if(group->audioType == frameType_unknown) {
      group->audioType = frame->type;
    }
    if(group->audioType != frame->type) {
      info.GetReturnValue().Set(false);
      return;
    }
    if(!ttLibC_FrameQueue_queue(group->audioQueue, frame)) {
      info.GetReturnValue().Set(false);
      return;
    }
    break;
  default:
    info.GetReturnValue().Set(false);
    return;
  }

  if(group->videoType != frameType_unknown) {
    if(group->audioType != frameType_unknown) {
      // audio & video
      while(true) {
        ttLibC_Frame *video = ttLibC_FrameQueue_ref_first(group->videoQueue);
        ttLibC_Frame *audio = ttLibC_FrameQueue_ref_first(group->audioQueue);
        if(video == NULL || audio == NULL) {
          break;
        }
        if(video->dts == 0 && video->pts != 0) {
          break;
        }
        if(video->dts > audio->pts) {
          audio = ttLibC_FrameQueue_dequeue_first(group->audioQueue);
          if(audio->type == frameType_aac) {
            // aacのasi情報は全部おくっておく。red5でなぜかうまく動作しないため
            ttLibC_AudioMessage *audioMessage = ttLibC_AudioMessage_addFrame(streamId, (ttLibC_Audio *)audio);
            if(audioMessage != NULL) {
              audioMessage->is_dsi_info = true;
              ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, audioMessage, sizeof(ttLibC_AudioMessage));
              ttLibC_AudioMessage_close(&audioMessage);
            }
          }
          ttLibC_AudioMessage *audioMessage = ttLibC_AudioMessage_addFrame(streamId, (ttLibC_Audio *)audio);
          if(audioMessage != NULL) {
            ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, audioMessage, sizeof(ttLibC_AudioMessage));
            ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
            ttLibC_AudioMessage_close(&audioMessage);
          }
        }
        else {
          video = ttLibC_FrameQueue_dequeue_first(group->videoQueue);
          ttLibC_VideoMessage *videoMessage = ttLibC_VideoMessage_addFrame(streamId, (ttLibC_Video *)video);
          ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, videoMessage, sizeof(ttLibC_VideoMessage));
          ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
          ttLibC_VideoMessage_close(&videoMessage);
        }
      }
    }
    else {
      // video only
      while(true) {
        ttLibC_Frame *video = ttLibC_FrameQueue_ref_first(group->videoQueue);
        if(video == NULL) {
          break;
        }
        if(video->dts == 0 && video->pts != 0) {
          break;
        }
        video = ttLibC_FrameQueue_dequeue_first(group->videoQueue);
        ttLibC_VideoMessage *videoMessage = ttLibC_VideoMessage_addFrame(streamId, (ttLibC_Video *)video);
        ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, videoMessage, sizeof(ttLibC_VideoMessage));
        ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
        ttLibC_VideoMessage_close(&videoMessage);
      }
    }
  }
  else {
    if(group->audioType != frameType_unknown) {
      // audio only
      while(true) {
        ttLibC_Frame *audio = ttLibC_FrameQueue_ref_first(group->audioQueue);
        if(audio == NULL) {
          break;
        }
        audio = ttLibC_FrameQueue_dequeue_first(group->audioQueue);
        if(audio->type == frameType_aac) {
          // aacのasi情報は全部おくっておく。red5でなぜかうまく動作しないため
          ttLibC_AudioMessage *audioMessage = ttLibC_AudioMessage_addFrame(streamId, (ttLibC_Audio *)audio);
          if(audioMessage != NULL) {
            audioMessage->is_dsi_info = true;
            ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, audioMessage, sizeof(ttLibC_AudioMessage));
            ttLibC_AudioMessage_close(&audioMessage);
          }
        }
        ttLibC_AudioMessage *audioMessage = ttLibC_AudioMessage_addFrame(streamId, (ttLibC_Audio *)audio);
        if(audioMessage != NULL) {
          ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, audioMessage, sizeof(ttLibC_AudioMessage));
          ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
          ttLibC_AudioMessage_close(&audioMessage);
        }
      }
    }
  }
  // この部分調整して、video only audio onlyとかでも動作可能にしておく。
  // このサンプルだとvideoとaudio両方あるタイプ
  info.GetReturnValue().Set(true);
}

NAN_METHOD(RtmpBootstrap::CloseStream) {
  // streamをcloseする。
  RtmpBootstrap *bootstrap = Nan::ObjectWrap::Unwrap<RtmpBootstrap>(info.Holder());
  if(bootstrap == NULL) {
    info.GetReturnValue().Set(false);
  }
  if(info.Length() != 1) {
    info.GetReturnValue().Set(false);
  }
  uint32_t streamId = info[0]->Uint32Value();
  ttLibC_Amf0Command *closeStream = ttLibC_Amf0Command_closeStream(streamId);
  ttLibC_TettyBootstrap_channels_write(bootstrap->bootstrap_, closeStream, sizeof(ttLibC_UserControlMessage));
  ttLibC_TettyBootstrap_channels_flush(bootstrap->bootstrap_);
  ttLibC_Amf0Command_close(&closeStream);
  info.GetReturnValue().Set(true);
}

Local<Value> RtmpBootstrap::toJsObject(ttLibC_Amf0Object *src_obj) {
  switch(src_obj->type) {
  case amf0Type_Number:
    return Nan::New(*((double *)src_obj->object));
  case amf0Type_Boolean:
    return Nan::New((bool)*((uint8_t *)src_obj->object) == 1);
  case amf0Type_String:
    return Nan::New((const char *)src_obj->object).ToLocalChecked();
  case amf0Type_Object:
    {
      Local<Object> object = Nan::New<Object>();
      ttLibC_Amf0MapObject *src_lists = (ttLibC_Amf0MapObject *)src_obj->object;
      for(int i = 0;src_lists[i].key != NULL && src_lists[i].amf0_obj != NULL;++ i) {
        Nan::Set(object, Nan::New(src_lists[i].key).ToLocalChecked(), toJsObject(src_lists[i].amf0_obj));
      }
      return object;
    }
    break;
  case amf0Type_MovieClip:
    break;
  case amf0Type_Null:
    break;
  case amf0Type_Undefined:
  case amf0Type_Reference:
    break;
  case amf0Type_Map:
    {
      Local<Object> object = Nan::New<Object>();
      ttLibC_Amf0MapObject *src_lists = (ttLibC_Amf0MapObject *)src_obj->object;
      for(int i = 0;src_lists[i].key != NULL && src_lists[i].amf0_obj != NULL;++ i) {
        Nan::Set(object, Nan::New(src_lists[i].key).ToLocalChecked(), toJsObject(src_lists[i].amf0_obj));
      }
      return object;
    }
    break;
  case amf0Type_ObjectEnd:
  case amf0Type_Array:
  case amf0Type_Date:
  case amf0Type_LongString:
  case amf0Type_Unsupported:
  case amf0Type_RecordSet:
  case amf0Type_XmlDocument:
  case amf0Type_TypedObject:
  case amf0Type_Amf3Object:
  default:
    break;
  }
  return Nan::New("undefined").ToLocalChecked();
}

bool RtmpBootstrap::frameManagerCloseCallback(void *ptr, void *key, void *item) {
  (void)ptr;
  (void)key;
  if(item != NULL) {
    ttLibC_FlvFrameManager *manager = (ttLibC_FlvFrameManager *)item;
    ttLibC_FlvFrameManager_close(&manager);
  }
  return true;
}

bool RtmpBootstrap::frameGroupCloseCallback(void *ptr, void *key, void *item) {
  (void)ptr;
  (void)key;
  if(item != NULL) {
    ttg_frameGroup *group = (ttg_frameGroup *)item;
    // groupが保持しているframeQueueを解放しなければならない。
    ttLibC_FrameQueue_close(&group->audioQueue);
    ttLibC_FrameQueue_close(&group->videoQueue);
    ttLibC_free(item);
  }
  return true;
}

RtmpBootstrap::RtmpBootstrap(Local<Value> address, Local<Value> app) : Bootstrap() {
  address_ = std::string(*String::Utf8Value(address->ToString()));
  app_ = std::string(*String::Utf8Value(app->ToString()));

  // ここでbootstrapの調整を実施する。
  ttLibC_TettyBootstrap_channel(bootstrap_, ChannelType_Tcp);
  ttLibC_TettyBootstrap_option(bootstrap_, Option_SO_KEEPALIVE);
  ttLibC_TettyBootstrap_option(bootstrap_, Option_TCP_NODELAY);

  // ここでpipelineを設定する
  handshake_ = (ttLibC_TettyChannelHandler *)ttLibC_RtmpHandshake_make();
  ttLibC_TettyBootstrap_pipeline_addLast(bootstrap_, handshake_);
  decoder_ = (ttLibC_TettyChannelHandler *)ttLibC_RtmpDecoder_make();
  ttLibC_TettyBootstrap_pipeline_addLast(bootstrap_, decoder_);
  encoder_ = (ttLibC_TettyChannelHandler *)ttLibC_RtmpEncoder_make();
  ttLibC_TettyBootstrap_pipeline_addLast(bootstrap_, encoder_);
  commandHandler_ = (ttLibC_TettyChannelHandler *)ttLibC_RtmpCommandHandler2_make(this);
  ttLibC_TettyBootstrap_pipeline_addLast(bootstrap_, commandHandler_);
  clientHandler_ = (ttLibC_TettyChannelHandler *)ttLibC_RtmpClientHandler2_make(this);
  ttLibC_TettyBootstrap_pipeline_addLast(bootstrap_, clientHandler_);

  streamIdFlvFrameManagerMap_ = ttLibC_StlMap_make();
  frameGroupMap_ = ttLibC_StlMap_make();
}

RtmpBootstrap::~RtmpBootstrap() {
  socket_.Reset();
  // ここですべてのstreamを解放しておく必要がある。
  ttLibC_StlMap_forEach(frameGroupMap_, frameGroupCloseCallback, NULL);
  ttLibC_StlMap_close(&frameGroupMap_);
  ttLibC_StlMap_forEach(streamIdFlvFrameManagerMap_, frameManagerCloseCallback, NULL);
  ttLibC_StlMap_close(&streamIdFlvFrameManagerMap_);
  // ここでpipelineのオブジェクトを解放しておかないといけない。
  ttLibC_RtmpHandshake_close((ttLibC_RtmpHandshake **)&handshake_);
  ttLibC_RtmpDecoder_close((ttLibC_RtmpDecoder **)&decoder_);
  ttLibC_RtmpEncoder_close((ttLibC_RtmpEncoder **)&encoder_);
  ttLibC_RtmpCommandHandler_close((ttLibC_RtmpCommandHandler **)&commandHandler_);
  ttLibC_RtmpClientHandler_close((ttLibC_RtmpClientHandler **)&clientHandler_);
}
