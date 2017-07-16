var ttLibJsGyp = require("bindings")("ttLibJsGyp");

module.exports = {
  Frame: ttLibJsGyp.Frame,
  reader: {
    FlvReader: function() {
      return ttLibJsGyp.Reader("flv");
    },
    MkvReader: function() {
      return ttLibJsGyp.Reader("mkv");
    },
    WebmReader: function() {
      return ttLibJsGyp.Reader("webm");
    },
    Mp4Reader: function() {
      return ttLibJsGyp.Reader("mp4");
    },
    MpegtsReader: function() {
      return ttLibJsGyp.Reader("mpegts");
    }
  },
  writer: {
    FlvWriter: function(videoCodec, audioCodec) {
      return ttLibJsGyp.Writer("flv", videoCodec, audioCodec);
    },
    MkvWriter: function(unitDuration, ...codecs) {
      return ttLibJsGyp.Writer.apply(null, ["mkv", unitDuration, codecs]);
    },
    WebmWriter: function(unitDuration, ...codecs) {
      return ttLibJsGyp.Writer.apply(null, ["webm", unitDuration, codecs]);
    },
    Mp4Writer: function(unitDuration, ...codecs) {
      return ttLibJsGyp.Writer.apply(null, ["mp4", unitDuration, codecs]);
    },
    MpegtsWriter: function(unitDuration, ...codecs) {
      return ttLibJsGyp.Writer.apply(null, ["mpegts", unitDuration, codecs]);
    }
  },
  decoder: {
    AvcodecVideoDecoder: (function() {
      var name = "avcodec";
      var decoder = function(type, width, height) {
        return ttLibJsGyp.Decoder.apply(null, [name, {type: type, width: width, height: height}]);
      }
      decoder["enabled"] = ttLibJsGyp.Decoder.check(name);
      return decoder;
    })(),
    AvcodecAudioDecoder: (function() {
      var name = "avcodec";
      var decoder = function(type, sampleRate, channelNum) {
        return ttLibJsGyp.Decoder.apply(null, [name, {type: type, sampleRate: sampleRate, channelNum: channelNum}]);
      }
      decoder["enabled"] = ttLibJsGyp.Decoder.check(name);
      return decoder;
    })(),
  },
  encoder: {
    AudioConverterEncoder: (function() {
      var name = "audioConverter";
      var encoder = function(type, sampleRate, channelNum, bitrate) {
        return ttLibJsGyp.Encoder.apply(null, [name, {type: type, sampleRate: sampleRate, channelNum: channelNum, bitrate: bitrate}]);
      };
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    FaacEncoder: (function() {
      var name = "faac";
      var encoder = function(type, sampleRate, channelNum, bitrate) {
        return ttLibJsGyp.Encoder.apply(null, [name, {type: type, sampleRate: sampleRate, channelNum: channelNum, bitrate: bitrate}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    Mp3lameEncoder: (function() {
      var name = "mp3lame";
      var encoder = function(sampleRate, channelNum, quality) {
        return ttLibJsGyp.Encoder.apply(null, [name, {sampleRate: sampleRate, channelNum: channelNum, quality:quality}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    JpegEncoder: (function() {
      var name = "jpeg";
      var encoder = function(width, height, quality) {
        return ttLibJsGyp.Encoder.apply(null, [name, {width: width, height: height, quality: quality}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    Openh264Encoder: (function() {
      var name = "openh264";
      var encoder = function(width, height, param, spatialParamArray) {
        return ttLibJsGyp.Encoder.apply(null, [name, {width: width, height: height, param: param, spatialParamArray: spatialParamArray}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    OpusEncoder: (function() {
      var name = "opus";
      var encoder = function(sampleRate, channelNum, unitSampleNum) {
        return ttLibJsGyp.Encoder.apply(null, [name, {sampleRate: sampleRate, channelNum: channelNum, unitSampleNum: unitSampleNum}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    TheoraEncoder: (function() {
      var name = "theora";
      var encoder = function(width, height, quality, bitrate, keyFrameInterval) {
        return ttLibJsGyp.Encoder.apply(null, [name, {width: width, height: height, quality: quality, bitrate: bitrate, keyFrameInterval: keyFrameInterval}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    VtCompressSessionEncoder: (function() {
      var name = "vtCompressSession";
      var encoder = function(type, width, height, fps=15, bitrate=320000, isBaseline=true) {
        return ttLibJsGyp.Encoder.apply(null, [name, {type: type, width: width, height: height, fps: fps, bitrate: bitrate, isBaseline: isBaseline}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    X264Encoder: (function() {
      var name = "x264";
      var encoder = function(width, height, preset="", tune="", profile="", param={}) {
        if(!profile || profile == "") {
          profile = "baseline";
        }
        return ttLibJsGyp.Encoder.apply(null, [name, {width: width, height: height, preset: preset, tune: tune, profile: profile, param: param}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    X265Encoder: (function() {
      var name = "x265";
      var encoder = function(width, height, preset="", tune="", profile="", param={}) {
        if(!profile || profile == "") {
          profile = "main";
        }
        return ttLibJsGyp.Encoder.apply(null, [name, {width: width, height: height, preset: preset, tune: tune, profile: profile, param: param}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    MSAacEncoder: (function() {
      var name = "msAac";
      var encoder = function(sampleRate, channelNum, bitrate) {
        return ttLibJsGyp.Encoder.apply(null, [name, {sampleRate: sampleRate, channelNum: channelNum, bitrate: bitrate}]);
      }
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
    MSH264Encoder: (function() {
      var name = "msH264";
      var encoder = function(encoder, width, height, bitrate) {
        return ttLibJsGyp.Encoder.apply(null, [name, {encoder: encoder, width: width, height: height, bitrate: bitrate}]);
      }
      encoder["listEncoders"] = ttLibJsGyp.MsH264.listEncoders;
      encoder["enabled"] = ttLibJsGyp.Encoder.check(name);
      return encoder;
    })(),
  },
  resampler: {
    AudioResampler: (function() {
      var name = "audio";
      var resampler = function(type, subType, channelNum=0) {
        return ttLibJsGyp.Resampler.apply(null, [name, {type: type, subType: subType, channelNum: channelNum}]);
      }
      resampler["enabled"] = ttLibJsGyp.Resampler.check(name);
      return resampler;
    })(),
    ImageResampler: (function() {
      var name = "image";
      var resampler = function(type, subType) {
        return ttLibJsGyp.Resampler.apply(null, [name, {type: type, subType: subType}]);
      }
      resampler["enabled"] = ttLibJsGyp.Resampler.check(name);
      return resampler;
    })(),
/*  // imageResizerは放置しておく。とりあえずelectronで使う予定だったら、webGLでresize処理は済むし・・・
    ImageResizer: function(){
    },*/
    SoundtouchResampler: (function() {
      var name = "soundtouch";
      var resampler = function(sampleRate, channelNum) {
        return ttLibJsGyp.Resampler.apply(null, ["soundtouch", {sampleRate: sampleRate, channelNum: channelNum}]);
      }
      resampler["enabled"] = ttLibJsGyp.Resampler.check(name);
      return resampler;
    })(),
    SpeexdspResampler: (function() {
      var name = "speexdsp";
      var resampler = function(channelNum, inSampleRate, outSampleRate, quality) {
        return ttLibJsGyp.Resampler.apply(null, ["speexdsp", {channelNum: channelNum, inSampleRate: inSampleRate, outSampleRate: outSampleRate, quality: quality}]);
      }
      resampler["enabled"] = ttLibJsGyp.Resampler.check(name);
      return resampler;
    })(),
  },
  MsSetup: ttLibJsGyp.MsSetup,
  MsLoopback: ttLibJsGyp.MsLoopback,
  rtmp: require("./tsdist/rtmp")
};
