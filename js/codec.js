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
  if(!reader.readFrame(data, (err, frame) => {
    return writer.writeFrame(frame, (err, data) => {
      return true;
    });
  })) {
    console.log("readFrame失敗");
    readableStream.close();
  }
});
// */

var videoDecoder = new tt.decoder.AvcodecVideoDecoder("h264", 640, 360);
var openh264Encoder = new tt.encoder.Openh264Encoder(
  640,
  360,
  {},[]
/*  {
    "iMinQp": 4,
    "iMaxQp": 51,
    "iTargetBitrate": 300000,
    "iMaxBitrate": 310000,
  },
  [
    {
      "iSpatialBitrate": 300000,
      "iMaxSpatialBitrate": 310000,
      "sSliceArgument.uiSliceMode": "SM_SINGLE_SLICE"
    }
  ]*/
  );
var x264Encoder = new tt.encoder.X264Encoder(
  640,
  360,
  "",
  "",
  "",
  {});
var x265Encoder = new tt.encoder.X265Encoder(
  640,
  360,
  "",
  "",
  "",
  {});
var theoraEncoder = new tt.encoder.TheoraEncoder(
  640, 360, 25, 320000, 15);
var imageResampler = new tt.resampler.ImageResampler(
  "bgr", "bgr");
var jpegEncoder = new tt.encoder.JpegEncoder(
  640, 360, 90);
var vtEncoder = new tt.encoder.VtCompressSessionEncoder(
  "h264", 640, 360
);

readableStream.on("data", (data) => {
  if(!reader.readFrame(data, (err, frame) => {
    if(frame.type == "h264") {
      return videoDecoder.decode(frame, (err, frame) => {
/*        return openh264Encoder.encode(frame, (err, frame) => {
          console.log(frame);
          return true;
        });*/
/*        return x264Encoder.encode(frame, (err, frame) => {
          console.log(frame);
          return true;
        });*/
/*        return x265Encoder.encode(frame, (err, frame) => {
          console.log(frame);
          return true;
        });*/
/*        return theoraEncoder.encode(frame, (err, frame) => {
          console.log(frame);
          return true;
        });*/
/*        return imageResampler.resample(frame, (err, frame) => {
          console.log(frame);
          return true;
        });*/
/*        return jpegEncoder.encode(frame, (err, frame) => {
          console.log(frame);
          return true;
        });*/
        return vtEncoder.encode(frame, (err, frame) => {
          console.log(frame);
          return true;
        });
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
