console.log("avcodecでいろいろとデコードしてみる動作テスト");

var fs = require('fs');
var mkv = require('../../').mkv;
var decoder = require('../../').decoder;
var resampler = require("../../").resampler;
var toBgr = new resampler.Image("bgr", "bgr");
var util = require("../../").util;
var window = new util.OpencvWindow("test");

var start = function() {
  return new Promise(function(resolve, reject) {
    resolve();
  });
}
var h264DecodeTest = function() {
  return new Promise(function(resolve, reject) {
    var reader = new mkv.Reader();
    var h264Decoder = new decoder.AvcodecVideo(640, 360, 'h264');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.mkv");
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        if(frame.type == 'h264') {
          h264Decoder.decode(frame, function(err, frame) {
            toBgr.resample(frame, function(err, frame) {
              window.show(frame);
              window.update(1);
            })
          });
        }
      });
    });
    readableStream.on('end', function() {
      resolve();
    });
  });
}
var h265DecodeTest = function() {
  return new Promise(function(resolve, reject) {
    var reader = new mkv.Reader();
    var h265Decoder = new decoder.AvcodecVideo(640, 360, 'h265');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h265.mp3.mkv");
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        if(frame.type == 'h265') {
          h265Decoder.decode(frame, function(err, frame) {
            toBgr.resample(frame, function(err, frame) {
              window.show(frame);
              window.update(1);
            })
          });
        }
      });
    });
    readableStream.on('end', function() {
      resolve();
    });
  });
}
var vp8DecodeTest = function() {
  return new Promise(function(resolve, reject) {
    var reader = new mkv.Reader();
    var vp8Decoder = new decoder.AvcodecVideo(640, 360, 'vp8');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.vp8.vorbis.webm");
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        if(frame.type == 'vp8') {
          vp8Decoder.decode(frame, function(err, frame) {
            toBgr.resample(frame, function(err, frame) {
              window.show(frame);
              window.update(1);
            })
          });
        }
      });
    });
    readableStream.on('end', function() {
      resolve();
    });
  });
}
var vp9DecodeTest = function() {
  return new Promise(function(resolve, reject) {
    var reader = new mkv.Reader();
    var vp9Decoder = new decoder.AvcodecVideo(640, 360, 'vp9');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.vp9.opus.webm");
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        if(frame.type == 'vp9') {
          vp9Decoder.decode(frame, function(err, frame) {
            toBgr.resample(frame, function(err, frame) {
              window.show(frame);
              window.update(1);
            })
          });
        }
      });
    });
    readableStream.on('end', function() {
      resolve();
    });
  });
}
var mjpegDecodeTest = function() {
  return new Promise(function(resolve, reject) {
    var reader = new mkv.Reader();
    var jpegDecoder = new decoder.AvcodecVideo(640, 360, 'jpeg');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.mjpeg.adpcmimawav.mkv");
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        if(frame.type == 'jpeg') {
          jpegDecoder.decode(frame, function(err, frame) {
            toBgr.resample(frame, function(err, frame) {
              window.show(frame);
              window.update(1);
            })
          });
        }
      });
    });
    readableStream.on('end', function() {
      resolve();
    });
  });
}
var theoraDecodeTest = function() {
  return new Promise(function(resolve, reject) {
    var reader = new mkv.Reader();
    var theoraDecoder = new decoder.AvcodecVideo(640, 360, 'theora');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.theora.speex.mkv");
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        if(frame.type == 'theora') {
          theoraDecoder.decode(frame, function(err, frame) {
            toBgr.resample(frame, function(err, frame) {
              window.show(frame);
              window.update(1);
            })
          });
        }
      });
    });
    readableStream.on('end', function() {
      resolve();
    });
  });
}
start()
//.then(h264DecodeTest)
//.then(h265DecodeTest)
//.then(vp8DecodeTest)
//.then(vp9DecodeTest)
//.then(mjpegDecodeTest)
.then(theoraDecodeTest)
.then(function() {
  console.log("all done.");
});