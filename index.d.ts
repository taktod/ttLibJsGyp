/// <reference types="node" />
// ttLibJsGyp.d.ts
declare module 'ttlibjsgyp'{
  export class value {
    public splitTypeKey: number;
    public splitTypeInner: number;
    public splitTypeP: number;
    public splitTypeDB: number;
    public splitTypeB: number;
    public splitTypeAllKey: number;
  }
  export class JsFrame {
    public type:string;
    public pts:number;
    public timebase:number;
    public data:Buffer;
    public is_non_copy:boolean;
    public id:number;
  }
  export class JsVideoFrame extends JsFrame {
    public width:number;
    public height:number;
    public videoType:string;
  }
  export class JsAudioFrame extends JsFrame {
    public sampleRate:number;
    public sampleNum:number;
    public channelNum:number;
  }
  export namespace flv {
    /**
     * flvデータを読み込みクラス
     */
    export class Reader {
      /**
       * コンストラクタ
       */
      constructor();
      /**
       * データを読み込む
       * @param data Bufferのbinaryデータ
       * @param func 結果を受け取るcallback関数
       * @return true:成功時 false:失敗時
       */
      read(data:Buffer, func:{(err:string, frame:JsFrame):void}):boolean;
    }
    /**
     * flvデータ書き出しクラス
     */
    export class Writer {
      /**
       * コンストラクタ
       * @param video_type "h264"や"flv1"といった映像フレームタイプ指定
       * @param audio_type "aac"や"mp3"といった音声フレームタイプ指定
       */
      constructor(video_type: string, audio_type: string);
      /**
       * データを書き出す(実際はbinaryデータをcallbackで受け取る)
       */
      write(frame:JsFrame, func:{(err:string, buffer:Buffer):void}):boolean;
    }
  }
  export namespace mpegts {
    /**
     * mpegtsデータ読み込みクラス
     */
    export class Reader {
      /**
       * コンストラクタ
       */
      constructor();
      /**
       * データを読み込む
       * @param data Bufferのbinaryデータ
       * @param func 結果を受け取るcallback関数
       * @return true:成功時 false:失敗時
       */
      read(data:Buffer, func:{(err:string, frame:JsFrame):void}):boolean;
    }
    /**
     * mpegtsデータ書き出しクラス
     */
    export class Writer {
      public enableDts:boolean;
      public splitType:number;
      public pts:number;
      public timebase:number;
      /**
       * コンストラクタ
       * @param types "h264" "aac" "mp3"といったフレームタイプ指定。
       * 前から順にIDが0x100 0x101..と増えていきます。
       */
      constructor(...types: string[]);
      /**
       * データを書き出す(実際はbinaryデータをcallbackで受け取る)
       */
      write(frame:JsFrame, func:{(err:string, buffer:Buffer):void}):boolean;
      /**
       * sdt pat pmtの情報を書き出す。
       */
      writeInfo(func:{(err:string, buffer:Buffer):void}):boolean;
    }
  }
  export namespace mkv {
    /**
     * mkvデータ読み込みクラス
     */
    export class Reader {
      /**
       * コンストラクタ
       */
      constructor();
      /**
       * データを読み込む
       * @param data Bufferのbinaryデータ
       * @param func 結果を受け取るcallback関数
       * @return true:成功時 false:失敗時
       */
      read(data:Buffer, func:{(err:string, frame:JsFrame):void}):boolean;
    }
    /**
     * mkvデータ書き出しクラス
     */
    export class Writer {
      public enableDts:boolean;
      public splitType:number;
      public pts:number;
      public timebase:number;
      /**
       * コンストラクタ
       * @param types "h264" "vp8" "aac" "opus"といったフレームタイプ指定。
       * 前から順にIDが1 2と増えていきます。
       */
      constructor(...types: string[]);
      /**
       * データを書き出す(実際はbinaryデータをcallbackで受け取る)
       */
      write(frame:JsFrame, func:{(err:string, buffer:Buffer):void}):boolean;
    }
  }
  export namespace mp4 {
    /**
     * mp4データ読み込みクラス
     */
    export class Reader {
      /**
       * コンストラクタ
       */
      constructor();
      /**
       * データを読み込む
       * @param data Bufferのbinaryデータ
       * @param func 結果を受け取るcallback関数
       * @return true:成功時 false:失敗時
       */
      read(data:Buffer, func:{(err:string, frame:JsFrame):void}):boolean;
    }
    /**
     * mp4データ書き出しクラス
     */
    export class Writer {
      public enableDts:boolean;
      public splitType:number;
      public pts:number;
      public timebase:number;
      /**
       * コンストラクタ
       * @param types "h264" "aac"といったフレームタイプ指定。
       * 前から順にIDが1,2,3...と増えていきます。
       */
      constructor(...types: string[]);
      /**
       * データを書き出す(実際はb inaryデータをcallbackで受け取る)
       */
      write(frame:JsFrame, func:{(err:string, buffer:Buffer):void}):boolean;
    }
  }
  export namespace webm {
    /**
     * webmデータ書き出しクラス
     */
    export class Writer {
      public enableDts:boolean;
      public splitType:number;
      public pts:number;
      public timebase:number;
      /**
       * コンストラクタ
       * @param types "vp8" "opus"といったフレームタイプ指定。
       * 前から順にIDが1 2と増えていきます。
       */
      constructor(...types: string[]);
      /**
       * データを書き出す(実際はbinaryデータをcallbackで受け取る)
       */
      write(frame:JsFrame, func:{(err:string, buffer:Buffer):void}):boolean;
    }
  }
  export namespace decoder {
    /**
     * OSXのAudioConverterによるデコード動作
     */
    export class AudioConverter {
      /**
       * コンストラクタ
       * @param sampleRate 動作サンプルレート
       * @param channelNum 動作チャンネル数
       * @param frameType  ターゲットフレームタイプ "aac"や"mp3"
       */
      constructor(sampleRate:number, channelNum:number, frameType:string);
      /**
       * デコードを実施します。
       * @param frame 変換元のフレーム
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      decode(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * ffmpegのavcodecによるデコード動作、今の所aacとmp3のデコード実績があります。
     */
    export class AvcodecAudio {
      /**
       * コンストラクタ
       * @param sampleRate 動作サンプルレート
       * @param channelNum 動作チャンネル数
       * @param frameType  ターゲットフレームタイプ "aac"や"mp3"
       */
      constructor(sampleRate:number, channelNum:number, frameType:string);
      /**
       * デコードを実施します。
       * @param frame 変換元のフレーム
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      decode(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * ffmpegのaavcodecによるデコード動作、今の所 flv1とvp8のデコード実績があります。
     */
    export class AvcodecVideo {
      /**
       * コンストラクタ
       * @param width     横幅
       * @param height    縦幅
       * @param frameType ターゲットフレームタイプ "flv1"や"vp8"
       */
      constructor(width:number, height:number, frameType:string);
      /**
       * デコードを実施します。
       * @param frame 変換元のフレーム
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      decode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * openh264によるh264のデコード動作
     */
    export class Openh264 {
      /**
       * コンストラクタ
       */
      constructor();
      /**
       * デコードを実施します。
       * @param frame 変換元のフレーム
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      decode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * libopusによるopusのデコード動作
     */
    export class Opus {
      /**
       * コンストラクタ
       * @param sampleRate 動作サンプルレート
       * @param channelNum 動作チャンネル数
       */
      constructor(sampleRate:number, channelNum:number);
      /**
       * デコードを実施します。
       * @param frame 変換元のフレーム
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      decode(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * libtheoraによるtheoraのデコード動作
     */
    export class Theora {
      /**
       * コンストラクタ
       */
      constructor();
      /**
       * デコードを実施します。
       * @param frame 変換元のフレーム
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      decode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * libvorbisによるvorbisのデコード動作
     */
    export class Vorbis {
      /**
       * コンストラクタ
       */
      constructor();
      /**
       * デコードを実施します。
       * @param frame 変換元のフレーム
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      decode(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * OSXのVideoToolDecompressSessionによるデコード動作
     * いまのところ"h264"のデコード実績があります。たぶんjpegもいける。
     */
    export class VtDecompressSession {
      /**
       * コンストラクタ
       * @param frameType 動作対象のフレームタイプ
       */
      constructor(frameType:string);
      /**
       * デコードを実施します。
       * @param frame 変換元のフレーム
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      decode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
  }
  export namespace encoder {
    /**
     * libfaacによるエンコード動作
     */
    export class Faac {
      /**
       * コンストラクタ
       * @param aacType    "main" "low"等のaacのprofile設定
       * @param sampleRate 対象サンプルレート
       * @param channelNum 対象チャンネル数
       * @param bitrate    動作ビットレート bit/sec
       */
      constructor(aacType:string, sampleRate:number, channelNum:number, bitrate:number);
      /**
       * エンコードを実施します。
       * @param frame 生成元のフレームデータPcmS16のlittleEndian
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      encode(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * libjpegによるエンコード動作
     * ttLibCのコンパイル時にリンクがlibjpeg-turboになっている場合は、jpeg-turboでも動作可能でした。
     */
    export class Jpeg {
      /**
       * コンストラクタ
       * @param width   横幅
       * @param height  縦幅
       * @param quality 0 - 100 100が一番高画質
       */
      constructor(width:number, height:number, quality:number);
      /**
       * エンコードを実施します。
       * @param frame 生成元のフレームデータyuv420のplanar
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      encode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * libmp3lameによるmp3エンコード動作
     */
    export class Mp3lame {
      /**
       * コンストラクタ
       * @param sampleRate ターゲットサンプルレート
       * @param channelNum ターゲットチャンネル数
       * @param quality    動作クオリティー 0 - 10 2:near best 5:good fast 7:ok very fast
       * 数値が小さいほど、high qualityだったのか・・・
       */
      constructor(sampleRate:number, channelNum:number, quality:number);
      /**
       * エンコードを実施します。
       * @param frame 生成元のフレームデータPcmS16のlittleEndian
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      encode(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * openh264によるh264エンコード動作
     */
    export class Openh264 {
      /**
       * コンストラクタ
       * @param width        横幅
       * @param height       縦幅
       * @param maxQuantizer quantizer値の最大値
       * @param minQuantizer quantizer値の最小値
       * @param bitrate      動作bitrate bit/sec
       * quantizerの値は小さいほど高画質
       */
      constructor(width:number, height:number, maxQuantizer:number, minQuantizer:number, bitrate:number);
      /**
       * エンコードを実施します。
       * @param frame 生成元のフレームデータyuv420のplanar
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      encode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * libopusによるopusエンコード動作
     */
    export class Opus {
      /**
       * コンストラクタ
       * @param sampleRate    動作サンプルレート
       * @param channelNum    動作チャンネル数
       * @param unitSampleNum 出力フレームのサンプル数設定
       */
      constructor(sampleRate:number, channelNum:number, unitSampleNum:number);
      /**
       * エンコードを実施します。
       * @param frame 生成元のフレームデータPcmS16のlittleEndian
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      encode(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * libtheoraによるtheoraのencode動作
     */
    export class Theora {
      /**
       * コンストラクタ
       * @param width   横幅
       * @param height  縦幅
       * @param quality 変換クオリティ値 0 - 63 数値が大きいほど綺麗になる。
       * @param bitrate ビットレート値 bps
       * @param keyFrameInterval キーフレーム間隔、ただし、oggのpacketセットの数なので、これがそのままフレーム数になるわけではないっぽい。
       * あと激しくうごいたりしたら、キーフレームが数個連続することもある。
       */
      constructor(width:number, height:number, quality?:number, bitrate?:number, keyFrameInterval?:number);
      /**
       * エンコードを実施します。
       * @param frame 生成元のフレームデータyuv420のplanar
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      encode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * x264によるh264エンコード動作
     */
    export class X264 {
      /**
       * コンストラクタ
       * @param width        横幅
       * @param height       縦幅
       * @param maxQuantizer quantizer値の最大値
       * @param minQuantizer quantizer値の最小値
       * @param bitrate      動作bitrate bit/sec
       * quantizerの値は小さいほど高画質
       */
      constructor(width:number, height:number, maxQuantizer:number, minQuantizer:number, bitrate:number);
      /**
       * エンコードを実施します。
       * @param frame 生成元のフレームデータyuv420のplanar
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      encode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * x265によるh265エンコード動作
     */
    export class X265 {
      /**
       * コンストラクタ
       * @param width        横幅
       * @param height       縦幅
       * @param bitrate      動作bitrate bit/sec
       */
      constructor(width:number, height:number, bitrate:number);
      /**
       * エンコードを実施します。
       * @param frame 生成元のフレームデータyuv420のplanar
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      encode(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
  }
  export namespace net {
    export namespace RtmpClient {
      /**
       * rtmpのNetConnection
       */
      export class NetConnection {
        /**
         * コンストラクタ
         */
        constructor();
        /**
         * イベントをリスナを追加する。
         * @param target
         * @param listener
         */
        addEventListener(target:string, listener:{(e):void}):boolean;
        /**
         * 接続する
         */
        connect(address:string):boolean;
      }
      /**
       * rtmpに接続したら、つくるnetStream
       * このstreamを通して、映像データや音声データにアクセスする。
       */
      export class NetStream {
        /**
         * コンストラクタ
         * @param nc NetConnection.Connect.Successしたコネクションオブジェクト
         */
        constructor(nc:NetConnection);
        /**
         * playを開始する。
         * @param name     対象ストリーム名
         * @param hasVideo 映像トラックを取得するかどうか
         * @param hasAudio 音声トラックを取得するかどうか
         */
        play(name:string, hasVideo?:boolean, hasAudio?:boolean);
        /**
         * bufferLengthを設定する。
         * @param value 
         */
        setBufferLength(value:number);
        /**
         * イベントをリスナを追加する。
         * @param target
         * @param listener
         */
        addEventListener(target:string, listener:{(e):void}):boolean;
        /**
         * フレーム受け取り時に呼ばれるcallback
         */
        setFrameListener(listener:{(err:string, frame:JsFrame):void}):boolean;
      }
    }
  }
  export namespace resampler {
    /**
     * Audioのresampler動作
     * pcmS16とpcmF32のいろんなタイプのデータに互いに変換します。
     */
    export class Audio {
      /**
       * コンストラクタ
       * @param type    変換先フレームタイプ "pcmS16"か"pcmF32"
       * @param pcmType フレームの詳細設定 littleEndianとかinterleaveとか
       */
      constructor(type:string, pcmType:string);
      /**
       * リサンプル実施
       * @param frame 変換元のフレームデータ
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      resample(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * bgr画像をリサイズします。
     */
    export class BgrImageResizer {
      /**
       * コンストラクタ
       * @param type   bgrType
       * @param width  変換後横幅
       * @param height 変換後縦幅
       */
      constructor(type:string, width:number, height:number);
      /**
       * リサイズ実施
       * @param frame 変換元のyuvフレームデータ
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      resample(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * yuvとbgrの相互変換します。
     */
    export class Image {
      /**
       * コンストラクタ
       * @param type    bgrかyuv420か
       * @param subType それぞれのtypeのsubType設定
       */
      constructor(type:string, subType:string);
      /**
       * 変換実施
       * @param frame 変換元のyuvフレームデータ
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      resample(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
    /**
     * speexdspによる周波数変換
     */
    export class Speexdsp {
      /**
       * コンストラクタ
       * @param channelNum       動作チャンネル数
       * @param inputSampleRate  入力サンプルレート
       * @param outputSampleRate 出力サンプルレート
       * @param quality          変換quality 0 - 10 10が最高だったはず。
       */
      constructor(channelNum:number, inputSampleRate:number, outputSampleRate:number, quality:number);
      /**
       * リサンプル実施
       * @param frame 変換元のフレームデータ pcmS16のlittleEndianが対応
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      resample(frame:JsAudioFrame, func:{(err:string, frame:JsAudioFrame):void}):boolean;
    }
    /**
     * yuvの画像サイズ変換
     */
    export class YuvImageResizer {
      /**
       * コンストラクタ
       * @param type
       * @param width
       * @param height
       * @param isQuick
       */
      constructor(type:string, width:number, height:number, isQuick:boolean);
      /**
       * リサイズ実施
       * @param frame 変換元のyuvフレームデータ
       * @param func  生成データを受け取るcallback
       * @return true:成功 false:失敗
       */
      resample(frame:JsVideoFrame, func:{(err:string, frame:JsVideoFrame):void}):boolean;
    }
  }
  export namespace util {
    /**
     * OSXのaudioUnitを利用した音声再生
     */
    export class AudioUnitPlayer {
      /**
       * コンストラクタ
       * @param sampleRate
       * @param channelNum
       */
      constructor(sampleRate:number, channelNum:number);
      /**
       * 音声データを再生にまわします。
       * @param frame 再生するpcmS16データ(interleaveのみ対応)
       */
      queue(frame:JsAudioFrame);
      /**
       * 現在のptsの進行具合を参照します。
       */
      getPts();
      /**
       * プレーヤーのtimebaseを参照します。
       */
      getTimebase();
    }
    /**
     * opencvを利用したカメラキャプチャ動作
     */
    export class OpencvCapture {
      /**
       * コンストラクタ
       * @param cameraNum
       * @param width
       * @param height
       */
      constructor(cameraNum:number, width:number, height:number);
      /**
       * デバイスからデータを取り出します。
       * @param func 生成データを受け取るcallback
       */
      query(func:{(err:string, frame:JsVideoFrame):void});
    }
    /**
     * opencvを利用したbgrデータの表示
     */
    export class OpencvWindow {
      /**
       * コンストラクタ
       * @param name ウィンドウ名
       */
      constructor(name:string);
      /**
       * 表示する。
       * @param frame 描画するフレーム
       */
      show(frame:JsVideoFrame);
      /**
       * 描画更新とキーボード入力を確認する。
       * @param interval 入力待ちミリ秒数
       * @return 0:なにもなかった場合 それ以外:入力キーコード
       */
      update(interval:number):number;
    }
  }
}
