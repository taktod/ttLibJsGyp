var tt = require("../");

var fs = require("fs");

// flv
var reader = new tt.reader.FlvReader();
var writer = new tt.writer.FlvWriter("h264", "aac");
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.flv");
// */

/* // mkv
var reader = new tt.reader.MkvReader();
var writer = new tt.writer.MkvWriter(1000, "h264", "aac");
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.mkv");
// */

/* // webm
var reader = new tt.reader.WebmReader();
var writer = new tt.writer.WebmWriter(1000, "vp8", "vorbis");
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.vp8.vorbis.webm");
// */

/* // mp4
var reader = new tt.reader.Mp4Reader();
var writer = new tt.writer.Mp4Writer(1000, "h264", "aac");
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.mp4");
// */

/*// mpegts
var reader = new tt.reader.MpegtsReader();
var writer = new tt.writer.MpegtsWriter(90000, "h264", "aac");
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.ts");
// */

/*
readableStream.on("data", (data) => {
  if(!reader.readFrame(data, (frame) => {
    return writer.writeFrame(frame, (data) => {
      return true;
    });
  })) {
    console.log("readFrame失敗");
    readableStream.close();
  }
});
// */

var audioDecoder = new tt.decoder.AvcodecAudioDecoder("aac", 44100, 2);
var audioResampler = new tt.resampler.AudioResampler("pcmS16", "littleEndian");
/*
var mp3lameEncoder = new tt.encoder.Mp3lameEncoder(44100, 2, 5);
var faacEncoder = new tt.encoder.FaacEncoder("Low", 44100, 2, 96000);
*/
//var fdkaacEncoder = new tt.encoder.FaacEncoder("AOT_AAC_LC", 44100, 2, 96000);
//var speexdspResampler = new tt.resampler.SpeexdspResampler(2, 44100, 32000, 8);
var swresampleResampler = new tt.resampler.SwresampleResampler("pcmF32", "interleave", 44100, 2,"pcmS16", "littleEndian", 32000, 1);
//var opusEncoder = new tt.encoder.OpusEncoder(48000, 2, 480);
//var acEncoder = new tt.encoder.AudioConverterEncoder("aac", 44100, 2, 96000);
//var vorbisEncoder = new tt.encoder.VorbisEncoder(44100, 2);
var speexEncoder = new tt.encoder.SpeexEncoder(32000, 1);

var pts = 0;

readableStream.on("data", (data) => {
  if(!reader.readFrame(data, (frame) => {
    if(frame.type == "aac") {
      return audioDecoder.decode(frame, (frame) => {
        return swresampleResampler.resample(frame, (frame) => {
          frame.pts = pts;
          frame.timebase = frame.sampleRate;
          pts += frame.sampleNum;
          return speexEncoder.encode(frame, (frame) => {
            console.log(frame);
            return true;
          })
        });
/*        return audioResampler.resample(frame, (frame) => {
          return vorbisEncoder.encode(frame, (frame) => {
            console.log(frame);
            return true;
          });
          return speexdspResampler.resample(frame, (frame) => {
            return speexEncoder.encode(frame, (frame) => {
              console.log(frame);
              return true;
            })
          });
        });*/
      });
    }
    else {
      return true;
    }
  })) {
    console.log("readFrame失敗");
    readableStream.close();
  }
});
