/// <reference types="node"/>
declare module 'ttlibjsgyp2' {
  /**
   * Frame object
   */
  export class Frame {
    /** type of frame */
    type:string;
    /** presentation timestamp */
    pts:number;
    /** timebase */
    timebase:number;
    /** id for frame. */
    id:number;

    /** width for video frame */
    width?:number;
    /** height for video frame */
    height?:number;
    /** videoType string (key inner info) */
    videoType?:string;

    /** sampleRate for audio frame */
    sampleRate?:number;
    /** sampleNUm for audio frame */
    sampleNum?:number;
    /** channelNum for audio frame */
    channelNum?:number;

    /** bgrデータのデータ開始位置指定 */
    data?:number;
    /** pcm lチャンネルデータのデータ開始位置指定 */
    lData?:number;
    /** pcm rチャンネルデータのデータ開始位置指定 */
    rData?:number;
    /** yuv yデータのdーた開始位置指定 */
    yData?:number;
    /** yuv uデータのdーた開始位置指定 */
    uData?:number;
    /** yuv vデータのdーた開始位置指定 */
    vData?:number;
    /** bgr stride値 */
    stride?:number;
    /** yuv yStride値 */
    yStride?:number;
    /** yuv uStride値 */
    uStride?:number;
    /** yuv vStride値 */
    vStride?:number;
    /** 特定のframe用 subType情報 */
    subType?:string;
    /** flv1 h264 h265用 該当フレームが破棄可能かフラグ */
    isDisposable?:boolean;
    /**
     * frameのbinaryデータを参照する
     * @return Buffer
     */
    getBinaryBuffer():Buffer;
    /**
     * frameのcloneを作成する。
     * @return Frame
     */
    clone():Frame;
    /**
     * binaryデータからframeを復元する。
     * @param prevFrame
     * @param binary    フレーム復元に利用するbinaryデータ
     * @param params    フレーム復元実施に必要な追加情報
     * @return Frame
     */
    static fromBinaryBuffer(
      prevFrame:Frame,
      binary:Buffer|DataView|ArrayBuffer,
      params:{}
    ):Frame;
  }
  export namespace reader {
    /**
     * flvデータを読み込む
     */
    export class FlvReader {
      constructor();
      /**
       * binaryデータからFrameを読み込む
       * @param data 読み込み対象のデータ
       * @param func データが見つかった時のcallback
       * @return
       */
      readFrame(
        data:Buffer,
        func:{(err:string, frame:Frame):boolean}
      ):boolean;
    }
    /**
     * matroskaデータを読み込む
     */
    export class MkvReader {
      constructor();
      /**
       * binaryデータからFrameを読み込む
       * @param data 読み込み対象のデータ
       * @param func データが見つかった時のcallback
       * @return
       */
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
    /**
     * mp4データを読み込む
     */
    export class Mp4Reader {
      constructor();
      /**
       * binaryデータからFrameを読み込む
       * @param data 読み込み対象のデータ
       * @param func データが見つかった時のcallback
       * @return
       */
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
    /**
     * mpegtsデータを読み込む
     */
    export class MpegtsReader {
      constructor();
      /**
       * binaryデータからFrameを読み込む
       * @param data 読み込み対象のデータ
       * @param func データが見つかった時のcallback
       * @return
       */
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
    /**
     * webmデータを読み込む
     */
    export class WebmReader {
      constructor();
      /**
       * binaryデータからFrameを読み込む
       * @param data 読み込み対象のデータ
       * @param func データが見つかった時のcallback
       * @return
       */
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
  }
  export namespace writer {
    /**
     * frameをflvデータとして書き出す
     */
    export class FlvWriter {
      /**
       * コンストラクタ
       * @param videoCodec 映像コーデック
       * @param audioCodec 音声コーデック
       */
      constructor(videoCodec:string, audioCodec:string);
      /**
       * フレームからbinaryを作成します
       * 音声フレームはid=8 映像フレームはid=9である必要があります
       * @param frame フレームを書き出す
       * @param func  生成されたbinaryデータをつけとるcallback
       * @return
       */
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      /**
       * 動作モードを設定します。
       * @param mode
       */
      setMode(mode:number):boolean;
    }
    export class MkvWriter {
      /**
       * コンストラクタ
       * @param unitDuration データの塊をどのくらいの大きさでつくるかミリ秒で指定します。
       * @param codecs 書き出しを実施するコーデック指定
       */
      constructor(unitDuration:number, ...codecs:string[]);
      /**
       * フレームからbinaryを作成します
       * コーデック指定に従って前から順にid=1 id=2と指定しなくてはなりません
       * @param frame フレームを書き出す
       * @param func  生成されたbinaryデータをつけとるcallback
       * @return
       */
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      /**
       * 動作モードを設定します。
       * @param mode
       */
      setMode(mode:number):boolean;
    }
    export class Mp4Writer {
      /**
       * コンストラクタ
       * @param unitDuration データの塊をどのくらいの大きさでつくるかミリ秒で指定します。
       * @param codecs 書き出しを実施するコーデック指定
       */
      constructor(unitDuration:number, ...codecs:string[]);
      /**
       * フレームからbinaryを作成します
       * コーデック指定に従って前から順にid=1 id=2と指定しなくてはなりません
       * @param frame フレームを書き出す
       * @param func  生成されたbinaryデータをつけとるcallback
       * @return
       */
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      /**
       * 動作モードを設定します。
       * @param mode
       */
      setMode(mode:number):boolean;
    }
    export class MpegtsWriter {
      /**
       * コンストラクタ
       * @param unitDuration データの塊をどのくらいの大きさでつくるか1/90000杪単位で指定します。
       * @param codecs 書き出しを実施するコーデック指定
       */
      constructor(unitDuration:number, ...codecs:string[]);
      /**
       * フレームからbinaryを作成します
       * コーデック指定に従って前から順にid=0x100 id=0x101と指定しなくてはなりません
       * @param frame フレームを書き出す
       * @param func  生成されたbinaryデータをつけとるcallback
       * @return
       */
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      /**
       * metaデータを作成します
       * @param func 生成されたbinaryデータをつけとるcallback
       */
      writeInfo(func:{(err:string, data:Buffer):boolean}):boolean;
      /**
       * 動作モードを設定します。
       * @param mode
       */
      setMode(mode:number):boolean;
    }
    export class WebmWriter {
      /**
       * コンストラクタ
       * @param unitDuration データの塊をどのくらいの大きさでつくるかミリ秒で指定します。
       * @param codecs 書き出しを実施するコーデック指定
       */
      constructor(unitDuration:number, ...codecs:string[]);
      /**
       * フレームからbinaryを作成します
       * コーデック指定に従って前から順にid=1 id=2と指定しなくてはなりません
       * @param frame フレームを書き出す
       * @param func  生成されたbinaryデータをつけとるcallback
       * @return
       */
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      /**
       * 動作モードを設定します。
       * @param mode
       */
      setMode(mode:number):boolean;
    }
  }
  export namespace decoder {
    export class AvcodecVideoDecoder {
      constructor(type:string, width:number, height:number);
      decode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class AvcodecAudioDecoder {
      constructor(type:string, sampleRate:number, channelNum:number);
      decode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
  }
  export namespace encoder {
    /**
     * osxのaudioConverterによるaacやmp3のencode動作
     */
    export class AudioConverterEncoder {
      constructor(type:string, sampleRate:number, channelNum:number, bitrate:number);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class FaacEncoder {
      constructor(type:string, sampleRate:number, channelNum:number, bitrate:number);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class Mp3lameEncoder {
      constructor(sampleRate:number, channelNum:number, quality:number);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class JpegEncoder {
      constructor(width:number, height:number, quality:number);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class Openh264Encoder {
      constructor(width:number, height:number, param:{}, spatialParamArray:{}[]);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class OpusEncoder {
      constructor(sampleRate:number, channelNum:number, unitSampleNum:number);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class TheoraEncoder {
      constructor(width:number, height:number, quality:number, bitrate:number, keyFrameInterval:number);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    /**
     * osxのvideoToolboxによるh264やjpegのencode動作
     */
    export class VtCompressSessionEncoder {
      constructor(type:string, width:number, height:number, fps?:number, bitrate?:number, isBaseline?:boolean);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class X264Encoder {
      constructor(width:number, height:number, preset?:string, tune?:string, profile?:string, params?:{});
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class X265Encoder {
      constructor(width:number, height:number, preset?:string, tune?:string, profile?:string, params?:{});
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    /**
     * windowsのaacEncoder
     */
    export class MSAacEncoder {
      constructor(sampleRate:number, channelNum:number, bitrate:number);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    /**
     * windowsのh264Encoder
     */
    export class MSH264Encoder {
      constructor(encoder:string, width:number, height:number, bitrate:number);
      encode(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static listEncoders(func:{(err:string, encoder:string):boolean}):boolean;
      static enabled:boolean;
    }
  }
  export namespace resampler {
    export class AudioResampler {
      constructor(type:string, subType:string, channelNum?:number);
      resample(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class ImageResampler {
      constructor(type:string, subType:string);
      resample(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class SoundtouchResampler {
      constructor(sampleRate:number, channelNum:number);
      resample(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
    export class SpeexdspResampler {
      constructor(channelNum:number, inSampleRate:number, outSampleRate:number, quality:number);
      resample(frame:Frame, func:{(err:string, frame:Frame):boolean}):boolean;
      static enabled:boolean;
    }
/*
    export class ImageResizer {

    }
    export class YuvImageResizer {

    }
    export class YuvImageRotater {

    }
    export class SwscaleResizer {

    }
    export class SwresampleResampler {

    }
*/
  }
  export class MsSetup {
    static CoInitialize(type?:string):boolean;
    static CoUninitialize():void;
    static MFStartup():boolean;
    static MFShutdown():void;
    static setlocale(locale:string):boolean;
  }
  export class MsLoopback {
    constructor(locale?:string, targetDevice?:string);
    querySring(func:{(err:string, frame:Frame):boolean}):boolean;
  }
  export namespace rtmp {
    export class NetConnection {
      constructor();
      connect(address:string):void;
      /**
       * eventにonStatusEventをいれる
       */
      on(event:string, func:{(event:any):void}):void;
    }
    export class NetStream {
      constructor(nc:NetConnection);
      play(name:string, video?:boolean, audio?:boolean):void;
      publish(name:string):void;
      setBufferLength(length:number):void;
      queueFrame(jsFrame:Frame): void;
      /**
       * eventにonStatusEventかonFrameCallbackをいれる
       */
      on(event:string, func:{(event:any):void}|{(err:string,frame:Frame):void}):void;
      close():void;
    }
  }
}
