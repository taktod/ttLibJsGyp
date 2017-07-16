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
var audioDecoder = new tt.decoder.AvcodecAudioDecoder("aac", 44100, 2);
readableStream.on("data", (data) => {
  if(!reader.readFrame(data, (err, frame) => {
    if(frame.type == "h264") {
      return videoDecoder.decode(frame, (err, frame) => {
//        console.log(frame);
        return true;
      });
    }
    else if(frame.type == "aac") {
      return audioDecoder.decode(frame, (err, frame) => {
//        console.log(frame);
        return true;
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
