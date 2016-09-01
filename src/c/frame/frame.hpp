// frameの動作のwrapper
#include <nan.h>
#include <stdio.h>
#include <stdbool.h>
#include <ttLibC/frame/frame.h>
#include <ttLibC/util/stlMapUtil.h>

#include <map>

// 単にframeを並べる形にすると、どれが先にくるかわからないので、listで調整しなければならない。
class FramePassingWorker : public Nan::AsyncWorker {
public:
    static void Init();
    static void Close();
    FramePassingWorker(
            ttLibC_Frame *frame, // clone動作は内部でやったらいいと思う。
            Nan::Callback *callback);
    void Execute();
    void HandleOKCallback();
};

/**
 * jsの世界で利用できるframeオブジェクトをセットアップする
 */
bool setupJsFrameObject(
        v8::Local<v8::Object> jsFrame,
        ttLibC_Frame *frame);


// classでつくっとくか・・・
// その方が便利だろ
// このフレームマネージャーを割り当てておけば、frameデータがきちんと出来上がる的な感じにしておく
// わざわざttLibCのstlMapつかわなければいけない理由がわからないか・・・
// 普通にc++のstl::mapを使おう。
/*
想定の使い方はこんな感じ
frameManager = new FrameManager();
ttLibC_Frame *frame = frameManager.getFrame(jsFrame);
いらなくなったら
delete frameManager;
とする。
*/
// objectWrapを使うと確かにobjectを保存できるけど
class JsFrameManager {
public:
    JsFrameManager();
    ttLibC_Frame *getFrame(v8::Local<v8::Object> jsFrame);
    ~JsFrameManager();
private:
    // id -> frame(reuse)を保持する
//    std::map<uint32_t, ttLibC_Frame *> *frameMap_;
    ttLibC_StlMap *frameStlMap_;
};

/**
 * ttLibCの世界で利用できるframeオブジェクトをセットアップする。
 */
bool setupFrameObject(
        ttLibC_Frame *reuse_frame,
        v8::Local<v8::Object> jsFrame);
