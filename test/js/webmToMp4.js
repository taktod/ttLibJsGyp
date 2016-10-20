var ttg = require("../../");
var mkvReader = new ttg.mkv.Reader();
var fs = require("fs");
var readStream = fs.createReadStream("mario.webm");
var vp8Decoder = new ttg.decoder.AvcodecVideo(640, 360, "vp8");
var opusDecoder = new ttg.decoder.Opus(48000, 1);
var h264Encoder = new ttg.encoder.Openh264(640, 360, 40, 4, 640000);
var aacEncoder = new ttg.encoder.Faac("low", 48000, 1, 64000);
console.log(aacEncoder);
var mp4Writer = new ttg.mp4.Writer("h264", "aac");
var flvWriter = new ttg.flv.Writer("h264", "aac");
var mp4Write = fs.createWriteStream("hoge.mp4");
var flvWrite = fs.createWriteStream("hoge.flv");

readStream.on("data", function(chunk) {
  mkvReader.read(chunk, function(err, frame) {
    if(err) return;
    switch(frame.type) {
    case 'vp8':
      vp8Decoder.decode(frame, function(err, frame) {
        if(err) return;
//        console.log("ここーん");
        h264Encoder.encode(frame, function(err, frame) {
          if(err) return;
//          console.log("ここきてる");
//          console.log(frame);
          frame.id = 9;
          flvWriter.write(frame, function(err, chunk) {
            flvWrite.write(chunk);
          });
          frame.id = 1;
          mp4Writer.write(frame, function(err, chunk) {
            mp4Write.write(chunk);
          });
        });
      });
      break;
    case 'opus':
      opusDecoder.decode(frame, function(err, frame) {
        if(err) return;
        aacEncoder.encode(frame, function(err, frame) {
          if(err) return;
          frame.id = 8;
          flvWriter.write(frame, function(err, chunk) {
            flvWrite.write(chunk);
          });
          frame.id = 2;
          mp4Writer.write(frame, function(err, chunk) {
            mp4Write.write(chunk);
          });
        });
      });
      break;
    default:
      break;
    }
  });
});