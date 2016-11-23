#include "frame.hpp"

#include <ttLibC/allocator.h>
#include <ttLibC/frame/video/bgr.h>
#include <ttLibC/frame/video/flv1.h>
#include <ttLibC/frame/video/h264.h>
#include <ttLibC/frame/video/h265.h>
#include <ttLibC/frame/video/jpeg.h>
#include <ttLibC/frame/video/theora.h>
#include <ttLibC/frame/video/vp6.h>
#include <ttLibC/frame/video/vp8.h>
#include <ttLibC/frame/video/vp9.h>
#include <ttLibC/frame/video/yuv420.h>
#include <ttLibC/frame/audio/aac.h>
#include <ttLibC/frame/audio/adpcmImaWav.h>
#include <ttLibC/frame/audio/mp3.h>
#include <ttLibC/frame/audio/nellymoser.h>
#include <ttLibC/frame/audio/opus.h>
#include <ttLibC/frame/audio/pcmf32.h>
#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/frame/audio/speex.h>
#include <ttLibC/frame/audio/vorbis.h>
#include <ttLibC/util/hexUtil.h>
#include <pthread.h>
#include <list>

using namespace v8;

static std::list<ttLibC_Frame *> *frame_list = NULL;
static pthread_mutex_t frame_mutex;

// decodeしたものも結局frameオブジェクトなので、問題なくいけそう。
void FramePassingWorker::Init() {
    pthread_mutex_init(&frame_mutex, NULL);
    frame_list = new std::list<ttLibC_Frame *>();
}

void FramePassingWorker::Close() {
    pthread_mutex_destroy(&frame_mutex);
    // frame_listにデータが残っていたら、本来なら撤去しなければならない
    delete frame_list;
}

// FramePassingWorker絡み
FramePassingWorker::FramePassingWorker(
        ttLibC_Frame *frame,
        Nan::Callback *callback) :
            Nan::AsyncWorker(callback) {
    int r = pthread_mutex_lock(&frame_mutex);
    if(r != 0) {
        puts("failed lock.");
        return;
    }
//    frame_ = ttLibC_Frame_clone(NULL, frame);
    frame_list->push_back(ttLibC_Frame_clone(NULL, frame));
    r = pthread_mutex_unlock(&frame_mutex);
    if(r != 0) {
        puts("failed unlock.");
    }
}

void FramePassingWorker::Execute() {
}

void FramePassingWorker::HandleOKCallback() {
    int r = pthread_mutex_lock(&frame_mutex);
    if(r != 0) {
        puts("failed lock..");
        return;
    }
    // フレームデータを再取得しなければならない。
    ttLibC_Frame *frame = frame_list->front();
    if(frame != NULL) {
        frame_list->remove(frame);
        Local<Object> jsFrame = Nan::New<Object>();
        if(!setupJsFrameObject(
                jsFrame,
                frame)) {
            Local<Value> args[] = {
                Nan::New("Jsオブジェクト作成失敗").ToLocalChecked(),
                Nan::Null()};
            callback->Call(2, args);
        }
        else {
            Local<Value> args[] = {
                Nan::Null(),
                jsFrame};
            callback->Call(2, args);
        }
        ttLibC_Frame_close(&frame);
    }
    r = pthread_mutex_unlock(&frame_mutex);
    if(r != 0) {
        puts("failed unlock..");
    }
}

// JsFrameManager絡み
JsFrameManager::JsFrameManager() {
    // mapをつくっておく
//    frameMap_ = new std::map<uint32_t, ttLibC_Frame *>();
    frameStlMap_ = ttLibC_StlMap_make();
}

static bool JsManager_closeMap(void *ptr, void *key, void *item) {
    ttLibC_Frame *frame = (ttLibC_Frame *)item;
    ttLibC_Frame_close(&frame);
    return true;
}

JsFrameManager::~JsFrameManager() {
    // 保持しているframeを解放してまわる。
/*    std::map<uint32_t, ttLibC_Frame *>::iterator iter = frameMap_->begin();
    while(iter != frameMap_->end()) {
        uint32_t id = iter->first;
        ++ iter;
        ttLibC_Frame *frame = iter->second;
        printf("frame:%d\n", frame);
        ttLibC_Frame_close(&frame);
    }
    delete frameMap_;*/
    ttLibC_StlMap_forEach(frameStlMap_, JsManager_closeMap, NULL);
    ttLibC_StlMap_close(&frameStlMap_);
}

static bool checkElementStrCmp(Local<Object> object, const char *key, const char *value) {
    Local<Value> val = Nan::Get(object->ToObject(), Nan::New(key).ToLocalChecked()).ToLocalChecked();
    if(!val->IsString()) {
        // そもそも文字列ではない。
        return false;
    }
    String::Utf8Value str(val->ToString());
    return strcmp((const char *)(*str), value) == 0;
}

static uint64_t getElementNumber(Local<Object> object, const char *key) {
    Local<Value> val = Nan::Get(object->ToObject(), Nan::New(key).ToLocalChecked()).ToLocalChecked();
    if(!val->IsNumber()) {
        return 0; // 数値じゃなければとりあえず0を応答しておこうと思う。
    }
    printf("check:%f %llu %llu \n", (float)val->NumberValue(), (uint32_t)val->NumberValue(), (uint32_t)val->Uint32Value());
    return (uint64_t)val->NumberValue();
}

// フレームを参照する。
ttLibC_Frame *JsFrameManager::getFrame(
        Local<Object> jsFrame) {
    // pts timebase data noncopy idを復元しておく。
    // 音声ならさらに sampleRate channel sampleNum
    // width height videoType
    // h264ならh264Type
    // これらの情報を復元してから、フレームオブジェクトに戻す。
    // jsFrameから必要な情報を復元しなければならない。
    // はじめから必要になりそうな情報をすべて取得しておけばいいのか。
    uint64_t pts = getElementNumber(jsFrame, "pts");
    printf("pts check on getFrame:%llu\n", pts);
    uint32_t timebase = (uint32_t)getElementNumber(jsFrame, "timebase");
    uint32_t id = (uint32_t)getElementNumber(jsFrame, "id");
    uint64_t lid = (uint64_t)id;
    // 以下はいまのところ、復元に利用していない。
    // まぁframeによっては必須になるわけだが・・・
    uint32_t width = (uint32_t)getElementNumber(jsFrame, "width");
    uint32_t height = (uint32_t)getElementNumber(jsFrame, "height");
    uint32_t sample_rate = (uint32_t)getElementNumber(jsFrame, "sampleRate");
    uint32_t sample_num = (uint32_t)getElementNumber(jsFrame, "sampleNum");
    uint32_t channel_num = (uint32_t)getElementNumber(jsFrame, "channelNum");

    Local<Value> v8Data = Nan::Get(jsFrame->ToObject(), Nan::New("data").ToLocalChecked()).ToLocalChecked();
    uint8_t *data = (uint8_t *)node::Buffer::Data(v8Data->ToObject());
    size_t data_size = node::Buffer::Length(v8Data->ToObject());
    // bufferデータをなんとかする。
/*    std::map<uint32_t, ttLibC_Frame *>::iterator iter = frameMap_->find(id);
    ttLibC_Frame *prev_frame = NULL;
    if(iter != frameMap_->end()) {
        prev_frame = (ttLibC_Frame *)iter->second;
//        frameMap_->erase(id);
    }*/
    ttLibC_Frame *prev_frame = (ttLibC_Frame *)ttLibC_StlMap_get(frameStlMap_, (void *)lid);
    if(checkElementStrCmp(jsFrame, "type", "aac")) {
        // aac
        ttLibC_Aac *aac = ttLibC_Aac_getFrame(
            (ttLibC_Aac *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(aac != NULL) {
            aac->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, aac);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)aac));
            printf("ptscheckhoge:%llu\n", aac->inherit_super.inherit_super.pts);
            return (ttLibC_Frame *)aac;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "adpcmimawav")) {
        ttLibC_AdpcmImaWav *adpcm = ttLibC_AdpcmImaWav_make(
                (ttLibC_AdpcmImaWav *)prev_frame,
                sample_rate,
                sample_num,
                channel_num,
                data,
                data_size,
                true,
                pts,
                timebase);
        if(adpcm != NULL) {
            adpcm->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, adpcm);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)adpcm));
            return (ttLibC_Frame *)adpcm;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "mp3")) {
        ttLibC_Mp3 *mp3 = ttLibC_Mp3_getFrame(
            (ttLibC_Mp3 *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(mp3 != NULL) {
            mp3->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, mp3);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)mp3));
            return (ttLibC_Frame *)mp3;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "nellymoser")) {
        ttLibC_Nellymoser *nellymoser = ttLibC_Nellymoser_make(
                (ttLibC_Nellymoser *)prev_frame,
                sample_rate,
                sample_num,
                channel_num,
                data,
                data_size,
                true,
                pts,
                timebase);
        if(nellymoser != NULL) {
            nellymoser->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, nellymoser);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)nellymoser));
            return (ttLibC_Frame *)nellymoser;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "opus")) {
        ttLibC_Opus *opus = ttLibC_Opus_getFrame(
            (ttLibC_Opus *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(opus != NULL) {
            opus->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, opus);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)opus));
            return (ttLibC_Frame *)opus;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "pcmF32")) {
        ttLibC_PcmF32 *pcm = NULL;
        if(checkElementStrCmp(jsFrame, "pcmF32Type", "interleave")) {
            pcm = ttLibC_PcmF32_make(
                (ttLibC_PcmF32 *)prev_frame,
                PcmF32Type_interleave,
                sample_rate,
                sample_num,
                channel_num,
                (void *)data,
                (size_t)data_size,
                NULL,
                (uint32_t)sample_num * 4 * channel_num,
                NULL,
                0,
                true,
                pts,
                timebase);
            if(pcm != NULL) {
                pcm->l_data = (uint8_t *)pcm->inherit_super.inherit_super.data;
            }
        }
        else if(checkElementStrCmp(jsFrame, "pcmF32Type", "planar")) {
            pcm = ttLibC_PcmF32_make(
                (ttLibC_PcmF32 *)prev_frame,
                PcmF32Type_planar,
                sample_rate,
                sample_num,
                channel_num,
                data,
                data_size,
                NULL,
                sample_num * 4,
                NULL,
                sample_num * 4 * (channel_num - 1),
                true,
                pts,
                timebase);
            if(pcm != NULL) {
                pcm->l_data = (uint8_t *)pcm->inherit_super.inherit_super.data;
                if(channel_num == 2) {
                    pcm->r_data = (uint8_t *)pcm->inherit_super.inherit_super.data + sample_num * 4;
                }
            }
        }
        if(pcm != NULL) {
            pcm->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, pcm);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)pcm));
            return (ttLibC_Frame *)pcm;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "pcmS16")) {
        ttLibC_PcmS16 *pcm = NULL;
        if(checkElementStrCmp(jsFrame, "pcmS16Type", "littleEndian")) {
            pcm = ttLibC_PcmS16_make(
                (ttLibC_PcmS16 *)prev_frame,
                PcmS16Type_littleEndian,
                sample_rate,
                sample_num,
                channel_num,
                data,
                data_size,
                NULL,
                sample_num * 2 * channel_num,
                NULL,
                0,
                true,
                pts,
                timebase);
            if(pcm != NULL) {
                pcm->l_data = (uint8_t *)pcm->inherit_super.inherit_super.data;
            }
        }
        else if(checkElementStrCmp(jsFrame, "pcmS16Type", "bigEndian")) {
        }
        else if(checkElementStrCmp(jsFrame, "pcmS16Type", "littleEndian_planar")) {
            pcm = ttLibC_PcmS16_make(
                (ttLibC_PcmS16 *)prev_frame,
                PcmS16Type_littleEndian_planar,
                sample_rate,
                sample_num,
                channel_num,
                data,
                data_size,
                NULL,
                sample_num * 2,
                NULL,
                sample_num * 2 * (channel_num - 1),
                true,
                pts,
                timebase);
            if(pcm != NULL) {
                pcm->l_data = (uint8_t *)pcm->inherit_super.inherit_super.data;
                if(channel_num == 2) {
                    pcm->r_data = (uint8_t *)pcm->inherit_super.inherit_super.data + sample_num * 2;
                }
            }
        }
        else if(checkElementStrCmp(jsFrame, "pcmS16Type", "bigEndian_planar")) {
        }
        if(pcm != NULL) {
            pcm->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, pcm);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)pcm));
            return (ttLibC_Frame *)pcm;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "speex")) {
        ttLibC_Speex *speex = ttLibC_Speex_getFrame(
            (ttLibC_Speex *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(speex != NULL) {
            speex->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, speex);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)speex));
            return (ttLibC_Frame *)speex;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "vorbis")) {
        ttLibC_Vorbis *vorbis = ttLibC_Vorbis_getFrame(
            (ttLibC_Vorbis *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(vorbis != NULL) {
            vorbis->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, vorbis);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)vorbis));
            return (ttLibC_Frame *)vorbis;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "bgr")) {
        // bgr
        uint32_t width_stride = (uint32_t)getElementNumber(jsFrame, "widthStride");
        ttLibC_Bgr_Type type = BgrType_bgr;
        if(checkElementStrCmp(jsFrame, "bgrType", "bgr")) {
            type = BgrType_bgr;
        }
        else if(checkElementStrCmp(jsFrame, "bgrType", "bgra")) {
            type = BgrType_bgra;
        }
        else if(checkElementStrCmp(jsFrame, "bgrType", "abgr")) {
            type = BgrType_abgr;
        }
        else {
            puts("想定外のbgrTypeでした。");
            return NULL;
        }
        ttLibC_Bgr *bgr = ttLibC_Bgr_make(
            (ttLibC_Bgr *)prev_frame,
            type,
            width,
            height,
            width_stride,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(bgr != NULL) {
            bgr->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, bgr);
            return (ttLibC_Frame *)bgr;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "flv1")) {
        // flv1
        ttLibC_Flv1 *flv1 = ttLibC_Flv1_getFrame(
            (ttLibC_Flv1 *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(flv1 != NULL) {
            flv1->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, flv1);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)flv1));
            return (ttLibC_Frame *)flv1;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "h264")) {
        // h264
        ttLibC_H264 *h264 = ttLibC_H264_getFrame(
            (ttLibC_H264 *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(h264 != NULL) {
            h264->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, h264);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)h264));
            return (ttLibC_Frame *)h264;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "h265")) {
        // h265
        ttLibC_H265 *h265 = ttLibC_H265_getFrame(
            (ttLibC_H265 *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(h265 != NULL) {
            h265->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, h265);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)h265));
            return (ttLibC_Frame *)h265;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "jpeg")) {
        // jpeg
        ttLibC_Jpeg *jpeg = ttLibC_Jpeg_getFrame(
            (ttLibC_Jpeg *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(jpeg != NULL) {
            jpeg->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, jpeg);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)jpeg));
            return (ttLibC_Frame *)jpeg;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "theora")) {
        // theora
        ttLibC_Theora *theora = ttLibC_Theora_getFrame(
            (ttLibC_Theora *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(theora != NULL) {
            theora->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, theora);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)theora));
            return (ttLibC_Frame *)theora;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "vp6")) {
        // vp6
        ttLibC_Vp6 *vp6 = ttLibC_Vp6_getFrame(
            (ttLibC_Vp6 *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(vp6 != NULL) {
            vp6->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, vp6);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)vp6));
            return (ttLibC_Frame *)vp6;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "vp8")) {
        // vp8
        ttLibC_Vp8 *vp8 = ttLibC_Vp8_getFrame(
            (ttLibC_Vp8 *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(vp8 != NULL) {
            vp8->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, vp8);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)vp8));
            return (ttLibC_Frame *)vp8;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "vp9")) {
        // vp9
        ttLibC_Vp9 *vp9 = ttLibC_Vp9_getFrame(
            (ttLibC_Vp9 *)prev_frame,
            data,
            data_size,
            true,
            pts,
            timebase);
        if(vp9 != NULL) {
            vp9->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, vp9);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)vp9));
            return (ttLibC_Frame *)vp9;
        }
    }
    else if(checkElementStrCmp(jsFrame, "type", "yuv420")){
        uint32_t y_stride = (uint32_t)getElementNumber(jsFrame, "yStride");
        uint32_t u_stride = (uint32_t)getElementNumber(jsFrame, "uStride");
        uint32_t v_stride = (uint32_t)getElementNumber(jsFrame, "vStride");
        // bufferデータを復活させるわけだが・・・
        ttLibC_Yuv420 *yuv = NULL;
        if(checkElementStrCmp(jsFrame, "yuv420Type", "planar")) {
            // あとはここに処理を書いていけばよい。
            uint32_t y_data_pos = (uint32_t)getElementNumber(jsFrame, "yDataPos");
            uint32_t u_data_pos = (uint32_t)getElementNumber(jsFrame, "uDataPos");
            uint32_t v_data_pos = (uint32_t)getElementNumber(jsFrame, "vDataPos");
            yuv = ttLibC_Yuv420_make(
                (ttLibC_Yuv420 *)prev_frame,
                Yuv420Type_planar,
                width,
                height,
                data,
                data_size,
                data + y_data_pos,
                y_stride,
                data + u_data_pos,
                u_stride,
                data + v_data_pos,
                v_stride,
                true,
                pts,
                timebase);
        }
        else if(checkElementStrCmp(jsFrame, "yuv420Type", "semiPlanar")) {

        }
        else if(checkElementStrCmp(jsFrame, "yuv420Type", "planarYvu")) {

        }
        else if(checkElementStrCmp(jsFrame, "yuv420Type", "semiPlanarYvu")) {

        }
        if(yuv != NULL) {
            yuv->inherit_super.inherit_super.id = id;
            ttLibC_StlMap_put(frameStlMap_, (void *)lid, yuv);
//            frameMap_->insert(std::pair<uint32_t, ttLibC_Frame *>(id, (ttLibC_Frame *)yuv));
            return (ttLibC_Frame *)yuv;
        }
    }
    return NULL;
}

// フレームの定義として利用するプログラム
/*
 * フレームは以下のようにしておこう。
 * emscripten側では、そのままbinaryを使っていた。わざわざつくっておく必要があるかわからなかったため。
 * あと極力高速動作させるのにも必要だったし・・・
{
    type // 面倒なので、文字列でつくっておく。
    pts
    timebase
    data
    is_non_copy
}
{
    音声としては、追加として・・・
    sampleRate
    sampleNum
    channelNumがある。
}
{
    映像としては
    width
    height
    type(key inner info)という情報が必要になってくる。
}
// ttLibCFrameから必要なものがつくれるようにしておけばいいかな。
 */
/*
// ここでframeオブジェクトをつくるとどこで解放するのかが、微妙になるためやらない。
// これでデータの更新ができるのは確定なので、からのオブジェクトをいれてもらってそこにフレームの情報を追加していくみたいな方向でつくっていけばよい。
void updateObject(v8::Local<v8::Object> obj) {
    Nan::Set(obj, Nan::New("hage").ToLocalChecked(), Nan::New("hagepong").ToLocalChecked());
}
*/
bool setupJsFrameObject_common(
        Local<Object> jsFrame,
        ttLibC_Frame *frame,
        const char *type) {
    if(frame == NULL) {
        return false;
    }
    if(!jsFrame->IsObject()) {
        return false;
    }
    Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New(type).ToLocalChecked());
    Nan::Set(jsFrame, Nan::New("pts").ToLocalChecked(), Nan::New((double)(frame->pts)));
    Nan::Set(jsFrame, Nan::New("timebase").ToLocalChecked(), Nan::New(frame->timebase));
    if(frame->data != NULL) {
        // ここがNULLの場合はぬるぽがでて、動作しないのか・・・
        Nan::Set(jsFrame, Nan::New("data").ToLocalChecked(), Nan::CopyBuffer((char *)frame->data, frame->buffer_size).ToLocalChecked());
    }
    Nan::Set(jsFrame, Nan::New("is_non_copy").ToLocalChecked(), Nan::New(frame->is_non_copy));
    Nan::Set(jsFrame, Nan::New("id").ToLocalChecked(), Nan::New((uint32_t)frame->id));
    return true;
}

bool setupJsFrameObject_commonAudio(
        Local<Object> jsFrame,
        ttLibC_Audio *audio) {
    if(audio == NULL) {
        return false;
    }
    if(!jsFrame->IsObject()) {
        return false;
    }
    Nan::Set(jsFrame, Nan::New("sampleRate").ToLocalChecked(), Nan::New(audio->sample_rate));
    Nan::Set(jsFrame, Nan::New("sampleNum").ToLocalChecked(), Nan::New(audio->sample_num));
    Nan::Set(jsFrame, Nan::New("channelNum").ToLocalChecked(), Nan::New(audio->channel_num));
    return true;
}

bool setupJsFrameObject_commonVideo(
        Local<Object> jsFrame,
        ttLibC_Video *video) {
    if(video == NULL) {
        return false;
    }
    if(!jsFrame->IsObject()) {
        return false;
    }
    Nan::Set(jsFrame, Nan::New("width").ToLocalChecked(), Nan::New(video->width));
    Nan::Set(jsFrame, Nan::New("height").ToLocalChecked(), Nan::New(video->height));
    switch(video->type) {
    case videoType_key:
        Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("key").ToLocalChecked());
        break;
    case videoType_inner:
        Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("inner").ToLocalChecked());
        break;
    case videoType_info:
        Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("info").ToLocalChecked());
        break;
    }
    return true; 
}


/**
 * jsの世界で利用できるframeオブジェクトをセットアップする
 */
bool setupJsFrameObject(
        Local<Object> jsFrame,
        ttLibC_Frame *frame) {
    if(frame == NULL) {
        return false;
    }
    if(!jsFrame->IsObject()) {
        return false;
    }
    switch(frame->type) {
    case frameType_bgr:
        {
            setupJsFrameObject_common(jsFrame, frame, "bgr");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
            ttLibC_Bgr *bgr = (ttLibC_Bgr *)frame;
            switch(bgr->type) {
            case BgrType_bgr:
                Nan::Set(jsFrame, Nan::New("bgrType").ToLocalChecked(), Nan::New("bgr").ToLocalChecked());
                break;
            case BgrType_bgra:
                Nan::Set(jsFrame, Nan::New("bgrType").ToLocalChecked(), Nan::New("bgra").ToLocalChecked());
                break;
            case BgrType_abgr:
                Nan::Set(jsFrame, Nan::New("bgrType").ToLocalChecked(), Nan::New("abgr").ToLocalChecked());
                break;
            default:
                break;
            }
            Nan::Set(jsFrame, Nan::New("widthStride").ToLocalChecked(), Nan::New(bgr->width_stride));
        }
        break;
    case frameType_flv1:
        {
            setupJsFrameObject_common(jsFrame, frame, "flv1");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
            ttLibC_Flv1 *flv1 = (ttLibC_Flv1 *)frame;
            switch(flv1->type) {
            case Flv1Type_intra:
                Nan::Set(jsFrame, Nan::New("flv1Type").ToLocalChecked(), Nan::New("intra").ToLocalChecked());
                break;
            case Flv1Type_inner:
                Nan::Set(jsFrame, Nan::New("flv1Type").ToLocalChecked(), Nan::New("inner").ToLocalChecked());
                break;
            case Flv1Type_disposableInner:
                Nan::Set(jsFrame, Nan::New("flv1Type").ToLocalChecked(), Nan::New("disposableInner").ToLocalChecked());
                break;
            default:
                break;
            }
        }
        break;
    case frameType_h264:
        {
            setupJsFrameObject_common(jsFrame, frame, "h264");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
            ttLibC_H264 *h264 = (ttLibC_H264 *)frame;
            switch(h264->type) {
            case H264Type_configData:
                Nan::Set(jsFrame, Nan::New("h264Type").ToLocalChecked(), Nan::New("configData").ToLocalChecked());
                break;
            case H264Type_sliceIDR:
                Nan::Set(jsFrame, Nan::New("h264Type").ToLocalChecked(), Nan::New("sliceIDR").ToLocalChecked());
                break;
            case H264Type_slice:
                Nan::Set(jsFrame, Nan::New("h264Type").ToLocalChecked(), Nan::New("slice").ToLocalChecked());
                break;
            case H264Type_unknown:
            default:
                Nan::Set(jsFrame, Nan::New("h264Type").ToLocalChecked(), Nan::New("unknown").ToLocalChecked());
                break;
            }
            switch(h264->frame_type) {
            case H264FrameType_P: 
                Nan::Set(jsFrame, Nan::New("frameType").ToLocalChecked(), Nan::New("P").ToLocalChecked());
                break;
            case H264FrameType_B:
                Nan::Set(jsFrame, Nan::New("frameType").ToLocalChecked(), Nan::New("B").ToLocalChecked());
                break;
            case H264FrameType_I:
                Nan::Set(jsFrame, Nan::New("frameType").ToLocalChecked(), Nan::New("I").ToLocalChecked());
                break;
            case H264FrameType_SP:
                Nan::Set(jsFrame, Nan::New("frameType").ToLocalChecked(), Nan::New("SP").ToLocalChecked());
                break;
            case H264FrameType_SI:
                Nan::Set(jsFrame, Nan::New("frameType").ToLocalChecked(), Nan::New("SI").ToLocalChecked());
                break;
            default:
                break;
            }
            Nan::Set(jsFrame, Nan::New("disposable").ToLocalChecked(), Nan::New(h264->is_disposable));
        }
        break;
    case frameType_h265:
        {
            setupJsFrameObject_common(jsFrame, frame, "h265");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
            ttLibC_H265 *h265 = (ttLibC_H265 *)frame;
            switch(h265->type) {
            case H265Type_configData:
                Nan::Set(jsFrame, Nan::New("h265Type").ToLocalChecked(), Nan::New("configData").ToLocalChecked());
                break;
            case H265Type_sliceIDR:
                Nan::Set(jsFrame, Nan::New("h265Type").ToLocalChecked(), Nan::New("sliceIDR").ToLocalChecked());
                break;
            case H265Type_slice:
                Nan::Set(jsFrame, Nan::New("h265Type").ToLocalChecked(), Nan::New("slice").ToLocalChecked());
                break;
            case H265Type_unknown:
            default:
                Nan::Set(jsFrame, Nan::New("h265Type").ToLocalChecked(), Nan::New("unknown").ToLocalChecked());
                break;
            }
            switch(h265->frame_type) {
            case H265FrameType_P:
                Nan::Set(jsFrame, Nan::New("frameType").ToLocalChecked(), Nan::New("P").ToLocalChecked());
                break;
            case H265FrameType_B:
                Nan::Set(jsFrame, Nan::New("frameType").ToLocalChecked(), Nan::New("B").ToLocalChecked());
                break;
            case H265FrameType_I:
                Nan::Set(jsFrame, Nan::New("frameType").ToLocalChecked(), Nan::New("I").ToLocalChecked());
                break;
            default:
                break;
            }
            Nan::Set(jsFrame, Nan::New("disposable").ToLocalChecked(), Nan::New(h265->is_disposable));
        }
        break;
    case frameType_jpeg:
        {
            setupJsFrameObject_common(jsFrame, frame, "jpeg");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
        }
        break;
    case frameType_theora:
        {
            setupJsFrameObject_common(jsFrame, frame, "theora");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
        }
        break;
    case frameType_vp6:
        {
            setupJsFrameObject_common(jsFrame, frame, "vp6");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
        }
        break;
    case frameType_vp8:
        {
            setupJsFrameObject_common(jsFrame, frame, "vp8");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
        }
        break;
    case frameType_vp9:
        {
            setupJsFrameObject_common(jsFrame, frame, "vp9");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
        }
        break;
/*
    case frameType_wmv1:
    case frameType_wmv2:*/
    case frameType_yuv420:
        {
            // TODO 問題はこっちだ・・・dataの部分がnullの可能性がある
            // cloneしたフレームの場合は問題ないんだが・・・
            // そこを注意しておかないと、データが蒸発する懸念あり。
            setupJsFrameObject_common(jsFrame, frame, "yuv420");
            setupJsFrameObject_commonVideo(jsFrame, (ttLibC_Video *)frame);
            ttLibC_Yuv420 *yuv = (ttLibC_Yuv420 *)frame;
            switch(yuv->type) {
            case Yuv420Type_planar:
                Nan::Set(jsFrame, Nan::New("yuv420Type").ToLocalChecked(), Nan::New("planar").ToLocalChecked());
                break;
            case Yuv420Type_semiPlanar:
                Nan::Set(jsFrame, Nan::New("yuv420Type").ToLocalChecked(), Nan::New("semiPlanar").ToLocalChecked());
                break;
            case Yvu420Type_planar:
                Nan::Set(jsFrame, Nan::New("yuv420Type").ToLocalChecked(), Nan::New("planarYvu").ToLocalChecked());
                break;
            case Yvu420Type_semiPlanar:
                Nan::Set(jsFrame, Nan::New("yuv420Type").ToLocalChecked(), Nan::New("semiPlanarYvu").ToLocalChecked());
                break;
            default:
                break;
            }
            // これ・・・planarの場合のみになってる。
            Nan::Set(jsFrame, Nan::New("yStride").ToLocalChecked(), Nan::New(yuv->y_stride));
            Nan::Set(jsFrame, Nan::New("uStride").ToLocalChecked(), Nan::New(yuv->u_stride));
            Nan::Set(jsFrame, Nan::New("vStride").ToLocalChecked(), Nan::New(yuv->v_stride));
            if(frame->data == NULL) {
                switch(yuv->type) {
                case Yuv420Type_planar:
                    {
                        uint32_t y_size = yuv->inherit_super.height * yuv->y_stride;
                        uint32_t u_size = yuv->inherit_super.height / 2 * yuv->u_stride;
                        uint32_t v_size = yuv->inherit_super.height / 2 * yuv->v_stride;
                        uint32_t buffer_size = y_size + u_size + v_size;
                        uint8_t *buffer = (uint8_t *)ttLibC_malloc(buffer_size);
                        if(buffer != NULL) {
                            memcpy(buffer, yuv->y_data, y_size);
                            memcpy(buffer + y_size, yuv->u_data, u_size);
                            memcpy(buffer + y_size + u_size, yuv->v_data, v_size);
                            Nan::Set(jsFrame, Nan::New("data").ToLocalChecked(), Nan::CopyBuffer((char *)buffer, buffer_size).ToLocalChecked());
                            Nan::Set(jsFrame, Nan::New("yDataPos").ToLocalChecked(), Nan::New(0));
                            Nan::Set(jsFrame, Nan::New("uDataPos").ToLocalChecked(), Nan::New(y_size));
                            Nan::Set(jsFrame, Nan::New("vDataPos").ToLocalChecked(), Nan::New(y_size + u_size));
                            ttLibC_free(buffer);
                        }
                        else {
                            // ここにきたらエラー
                        }
                    }
                    break;
                case Yuv420Type_semiPlanar:
                case Yvu420Type_planar:
                case Yvu420Type_semiPlanar:
                default:
                    break;
                }
            }
            else {
                uint8_t *data = (uint8_t *)frame->data;
                Nan::Set(jsFrame, Nan::New("yDataPos").ToLocalChecked(), Nan::New((int)(yuv->y_data - data)));
                Nan::Set(jsFrame, Nan::New("uDataPos").ToLocalChecked(), Nan::New((int)(yuv->u_data - data)));
                Nan::Set(jsFrame, Nan::New("vDataPos").ToLocalChecked(), Nan::New((int)(yuv->v_data - data)));
            }
        }
        break;
    case frameType_aac:
        {
            setupJsFrameObject_common(jsFrame, frame, "aac");

            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);

            ttLibC_Aac *aac = (ttLibC_Aac *)frame;
            switch(aac->type) {
            case AacType_adts:
                Nan::Set(jsFrame, Nan::New("aacType").ToLocalChecked(), Nan::New("adts").ToLocalChecked());
                break;
            case AacType_dsi:
                Nan::Set(jsFrame, Nan::New("aacType").ToLocalChecked(), Nan::New("dsi").ToLocalChecked());
                break;
            case AacType_raw:
            default:
                // きちんとdsi情報が応答されていないreaderだとちょっとまずいことになる。
                // flv系はやっておいたけど・・・
                Nan::Set(jsFrame, Nan::New("aacType").ToLocalChecked(), Nan::New("raw").ToLocalChecked());
                uint32_t dsi_info;
                /*size_t sz = */ttLibC_Aac_readDsiInfo(aac, &dsi_info, 4);
                Nan::Set(jsFrame, Nan::New("dsiInfo").ToLocalChecked(), Nan::New(dsi_info));
                break;
            }
        }
        break;
    case frameType_adpcm_ima_wav:
        {
            setupJsFrameObject_common(jsFrame, frame, "adpcmimawav");
            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);
        }
        break;
    case frameType_mp3:
        {
            setupJsFrameObject_common(jsFrame, frame, "mp3");
            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);
            ttLibC_Mp3 *mp3 = (ttLibC_Mp3 *)frame;
            switch(mp3->type) {
            case Mp3Type_tag:
                Nan::Set(jsFrame, Nan::New("mp3Type").ToLocalChecked(), Nan::New("tag").ToLocalChecked());
                break;
            case Mp3Type_id3:
                Nan::Set(jsFrame, Nan::New("mp3Type").ToLocalChecked(), Nan::New("id3").ToLocalChecked());
                break;
            case Mp3Type_frame:
                Nan::Set(jsFrame, Nan::New("mp3Type").ToLocalChecked(), Nan::New("frame").ToLocalChecked());
                break;
            default:
                break;
            }
        }
        break;
    case frameType_nellymoser:
        {
            setupJsFrameObject_common(jsFrame, frame, "nellymoser");
            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);
        }
        break;
    case frameType_opus:
        {
            setupJsFrameObject_common(jsFrame, frame, "opus");
            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);
            ttLibC_Opus *opus = (ttLibC_Opus *)frame;
            switch(opus->type) {
                case OpusType_header:
                    Nan::Set(jsFrame, Nan::New("opusType").ToLocalChecked(), Nan::New("header").ToLocalChecked());
                    break;
                case OpusType_comment:
                    Nan::Set(jsFrame, Nan::New("opusType").ToLocalChecked(), Nan::New("comment").ToLocalChecked());
                    break;
                case OpusType_frame:
                    Nan::Set(jsFrame, Nan::New("opusType").ToLocalChecked(), Nan::New("frame").ToLocalChecked());
                    break;
                default:
                break;
            }
        }
        break;
//    case frameType_pcm_alaw:
    case frameType_pcmF32:
        {
            setupJsFrameObject_common(jsFrame, frame, "pcmF32");
            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);
            ttLibC_PcmF32 *pcm = (ttLibC_PcmF32 *)frame;
            switch(pcm->type) {
            case PcmF32Type_interleave:
                Nan::Set(jsFrame, Nan::New("pcmF32Type").ToLocalChecked(), Nan::New("interleave").ToLocalChecked());
                break;
            case PcmF32Type_planar:
                Nan::Set(jsFrame, Nan::New("pcmF32Type").ToLocalChecked(), Nan::New("planar").ToLocalChecked());
                break;
            default:
                break;
            }
            if(frame->data == NULL) {
                uint32_t buffer_size = pcm->l_stride;
                if(pcm->r_data != NULL) {
                    buffer_size += pcm->r_stride;
                }
                uint8_t *buffer = (uint8_t *)ttLibC_malloc(buffer_size);
                memcpy(buffer, pcm->l_data, pcm->l_stride);
                if(pcm->r_data != NULL) {
                    memcpy(buffer + pcm->l_stride, pcm->r_data, pcm->r_stride);
                }
                Nan::Set(jsFrame, Nan::New("data").ToLocalChecked(), Nan::CopyBuffer((char *)buffer, buffer_size).ToLocalChecked());
                ttLibC_free(buffer);
            }
        }
        break;
//    case frameType_pcm_mulaw:
    case frameType_pcmS16:
        {
            setupJsFrameObject_common(jsFrame, frame, "pcmS16");
            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);
            // とりあえずpcmデータは、dataのところに全データがあるのを前提としておいてよさげ。
            // avcodecの動作でこの幻想は打ち壊されるっぽいですね。こまったもんだ。
            ttLibC_PcmS16 *pcm = (ttLibC_PcmS16 *)frame;
            switch(pcm->type) {
            case PcmS16Type_littleEndian:
                Nan::Set(jsFrame, Nan::New("pcmS16Type").ToLocalChecked(), Nan::New("littleEndian").ToLocalChecked());
                break;
            case PcmS16Type_bigEndian:
                Nan::Set(jsFrame, Nan::New("pcmS16Type").ToLocalChecked(), Nan::New("bigEndian").ToLocalChecked());
                break;
            case PcmS16Type_littleEndian_planar:
                Nan::Set(jsFrame, Nan::New("pcmS16Type").ToLocalChecked(), Nan::New("littleEndian_planar").ToLocalChecked());
                break;
            case PcmS16Type_bigEndian_planar:
                Nan::Set(jsFrame, Nan::New("pcmS16Type").ToLocalChecked(), Nan::New("bigEndian_planar").ToLocalChecked());
                break;
            default:
                break;
            }
            if(frame->data == NULL) {
                uint32_t buffer_size = pcm->l_stride;
                if(pcm->r_data != NULL) {
                    buffer_size += pcm->r_stride;
                }
                uint8_t *buffer = (uint8_t *)ttLibC_malloc(buffer_size);
                memcpy(buffer, pcm->l_data, pcm->l_stride);
                if(pcm->r_data != NULL) {
                    memcpy(buffer + pcm->l_stride, pcm->r_data, pcm->r_stride);
                }
                Nan::Set(jsFrame, Nan::New("data").ToLocalChecked(), Nan::CopyBuffer((char *)buffer, buffer_size).ToLocalChecked());
                ttLibC_free(buffer);
            }
        }
        break;
    case frameType_speex:
        {
            setupJsFrameObject_common(jsFrame, frame, "speex");
            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);
            ttLibC_Speex *speex = (ttLibC_Speex *)frame;
            switch(speex->type) {
            case SpeexType_header:
                Nan::Set(jsFrame, Nan::New("speexType").ToLocalChecked(), Nan::New("header").ToLocalChecked());
                break;
            case SpeexType_comment:
                Nan::Set(jsFrame, Nan::New("speexType").ToLocalChecked(), Nan::New("comment").ToLocalChecked());
                break;
            case SpeexType_frame:
                Nan::Set(jsFrame, Nan::New("speexType").ToLocalChecked(), Nan::New("frame").ToLocalChecked());
                break;
            default:
                break;
            }
        }
        break;
    case frameType_vorbis:
        {
            setupJsFrameObject_common(jsFrame, frame, "vorbis");
            setupJsFrameObject_commonAudio(jsFrame, (ttLibC_Audio *)frame);
            ttLibC_Vorbis *vorbis = (ttLibC_Vorbis *)frame;
            switch(vorbis->type) {
            case VorbisType_identification:
                Nan::Set(jsFrame, Nan::New("vorbisType").ToLocalChecked(), Nan::New("identification").ToLocalChecked());
                break;
            case VorbisType_comment:
                Nan::Set(jsFrame, Nan::New("vorbisType").ToLocalChecked(), Nan::New("comment").ToLocalChecked());
                break;
            case VorbisType_setup:
                Nan::Set(jsFrame, Nan::New("vorbisType").ToLocalChecked(), Nan::New("setup").ToLocalChecked());
                break;
            case VorbisType_frame:
                Nan::Set(jsFrame, Nan::New("vorbisType").ToLocalChecked(), Nan::New("frame").ToLocalChecked());
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
    return true;
}

/**
 * ttLibCの世界で利用できるframeオブジェクトをセットアップする。
 */
bool setupFrameObject(
        ttLibC_Frame *reuse_frame,
        v8::Local<v8::Object> jsFrame) {
    if(!jsFrame->IsObject()) {
        return false;
    }
    // ないオブジェクトを参照したら、どうなるんだろう・・・
    return true;
}
