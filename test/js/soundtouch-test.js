// mkvデータの動作テスト
console.log("mkv絡みの動作テスト");
var fs = require('fs');
var mkv = require('../../').mkv;
var mkvReader = new mkv.Reader();
var mkvWriter = new mkv.Writer("h264", "aac");
var readableStream = fs.createReadStream(process.env["HOME"] + "/tools/data/source/test.h264.aac.mkv");
var writableStream = fs.createWriteStream(process.env["HOME"] + "/tools/data/node_out/test.h264.aac.mkv");
var decoder = require('../../').decoder;
var aacDecoder = new decoder.AvcodecAudio(44100, 2, "aac");
var resampler = require("../../").resampler;
var soundtouch = new resampler.Soundtouch(44100, 2);
soundtouch.setPitch(1.5);
var toPcmS16 = new resampler.Audio("pcmS16", "littleEndian");
var encoder = require("../../").encoder;
var aacEncoder = new encoder.Faac("low", 44100, 2, 96000);

var pts = 0;

readableStream.on('data', function(data) {
  mkvReader.read(data, function(err, frame) {
    if(frame.type == 'aac') {
      aacDecoder.decode(frame, function(err, frame) {
        soundtouch.resample(frame, function(err, frame) {
          frame.pts = pts;
          pts += frame.sampleNum;
          toPcmS16.resample(frame, function(err, frame) {
            aacEncoder.encode(frame, function(err, frame) {
              frame.id = 2;
              mkvWriter.write(frame, function(err, data) {
                writableStream.write(data);
              });
            })
          });
        });
      });
    }
    else {
      mkvWriter.write(frame, function(err, data) {
        writableStream.write(data);
      });
    }
  });
});
