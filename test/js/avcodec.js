console.log("avcodecでいろいろとデコードしてみる動作テスト");

var fs = require('fs');
var mkv = require('../../').mkv;
var decoder = require('../../').decoder;
var resampler = require("../../").resampler;
var toBgr = new resampler.Image("bgr", "bgr");
var toPcmS16 = new resampler.Audio("pcmS16", "littleEndian");
var util = require("../../").util;
var window = new util.OpencvWindow("test");

var start = function() {
  return new Promise(function(resolve, reject) {
    resolve();
  });
}
// 以下映像系
var h264DecodeTest = function() {
  return new Promise(function(resolve, reject) {
    console.log("h264");
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
    console.log("h265");
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
    console.log("vp8");
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
    console.log("vp9");
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
    console.log("jpeg");
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
    console.log("theora");
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
// 以下音声系

var aacDecodeTest = function() {
  return new Promise(function(resolve, reject) {
    console.log("aac");
    var counter = 0;
    var reader = new mkv.Reader();
    var aacDecoder = new decoder.AvcodecAudio(44100, 2, 'aac');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.aac.mkv");
    var player = new util.AudioUnitPlayer(44100, 2);
    var queue = [];
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        aacDecoder.decode(frame, function(err, frame) {
          toPcmS16.resample(frame, function(err, frame) {
            queue.push(frame);
          });
        });
      });
    });
    var work = function() {
      var noFrame = true;
      while(true) {
        var frame = queue.shift();
        if(frame != null) {
          noFrame = false;
          if(!player.queue(frame)) {
            queue.unshift(frame);
            break;
          }
        }
        else {
          break;
        }
      }
      if(noFrame)
        counter ++;
      else
        counter = 0;
      if(counter > 5) {
        resolve();
        return;
      }
      setTimeout(work, 10);
    };
    setTimeout(work, 1000);
  });
}
var mp3DecodeTest = function() {
  return new Promise(function(resolve, reject) {
    console.log("mp3");
    var counter = 0;
    var reader = new mkv.Reader();
    var mp3Decoder = new decoder.AvcodecAudio(44100, 2, 'mp3');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.mp3.mkv");
    var player = new util.AudioUnitPlayer(44100, 2);
    var queue = [];
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        mp3Decoder.decode(frame, function(err, frame) {
          toPcmS16.resample(frame, function(err, frame) {
            queue.push(frame);
          });
        });
      });
    });
    var work = function() {
      var noFrame = true;
      while(true) {
        var frame = queue.shift();
        if(frame != null) {
          noFrame = false;
          if(!player.queue(frame)) {
            queue.unshift(frame);
            break;
          }
        }
        else {
          break;
        }
      }
      if(noFrame)
        counter ++;
      else
        counter = 0;
      if(counter > 5) {
        resolve();
        return;
      }
      setTimeout(work, 10);
    };
    setTimeout(work, 1000);
  });
}
var adpcmimawavDecodeTest = function() {
  return new Promise(function(resolve, reject) {
    console.log("adpcmimawav");
    var counter = 0;
    var reader = new mkv.Reader();
    var adpcmimawavDecoder = new decoder.AvcodecAudio(44100, 2, 'adpcmimawav');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.adpcmimawav.mkv");
    var player = new util.AudioUnitPlayer(44100, 2);
    var queue = [];
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        adpcmimawavDecoder.decode(frame, function(err, frame) {
          toPcmS16.resample(frame, function(err, frame) {
            queue.push(frame);
          });
        });
      });
    });
    var work = function() {
      var noFrame = true;
      while(true) {
        var frame = queue.shift();
        if(frame != null) {
          noFrame = false;
          if(!player.queue(frame)) {
            queue.unshift(frame);
            break;
          }
        }
        else {
          break;
        }
      }
      if(noFrame)
        counter ++;
      else
        counter = 0;
      if(counter > 5) {
        resolve();
        return;
      }
      setTimeout(work, 10);
    };
    setTimeout(work, 1000);
  });
}
var opusDecodeTest = function() {
  return new Promise(function(resolve, reject) {
    console.log("opus");
    var counter = 0;
    var reader = new mkv.Reader();
    var opusDecoder = new decoder.AvcodecAudio(48000, 2, 'opus');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.opus.mkv");
    var player = new util.AudioUnitPlayer(48000, 2);
    var queue = [];
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        opusDecoder.decode(frame, function(err, frame) {
          toPcmS16.resample(frame, function(err, frame) {
            queue.push(frame);
          });
        });
      });
    });
    var work = function() {
      var noFrame = true;
      while(true) {
        var frame = queue.shift();
        if(frame != null) {
          noFrame = false;
          if(!player.queue(frame)) {
            queue.unshift(frame);
            break;
          }
        }
        else {
          break;
        }
      }
      if(noFrame)
        counter ++;
      else
        counter = 0;
      if(counter > 5) {
        resolve();
        return;
      }
      setTimeout(work, 10);
    };
    setTimeout(work, 1000);
  });
}
var speexDecodeTest = function() {
  return new Promise(function(resolve, reject) {
    console.log("speex");
    var counter = 0;
    var reader = new mkv.Reader();
    var speexDecoder = new decoder.AvcodecAudio(32000, 2, 'speex');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.speex.mkv");
    var player = new util.AudioUnitPlayer(32000, 2);
    var queue = [];
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        speexDecoder.decode(frame, function(err, frame) {
          toPcmS16.resample(frame, function(err, frame) {
            queue.push(frame);
          });
        });
      });
    });
    var work = function() {
      var noFrame = true;
      while(true) {
        var frame = queue.shift();
        if(frame != null) {
          noFrame = false;
          if(!player.queue(frame)) {
            queue.unshift(frame);
            break;
          }
        }
        else {
          break;
        }
      }
      if(noFrame)
        counter ++;
      else
        counter = 0;
      if(counter > 5) {
        resolve();
        return;
      }
      setTimeout(work, 10);
    };
    setTimeout(work, 1000);
  });
}
var vorbisDecodeTest = function() {
  return new Promise(function(resolve, reject) {
    console.log("vorbis");
    var counter = 0;
    var reader = new mkv.Reader();
    var vorbisDecoder = new decoder.AvcodecAudio(44100, 2, 'vorbis');
    var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.vorbis.mkv");
    var player = new util.AudioUnitPlayer(44100, 2);
    var queue = [];
    readableStream.on('data', function(data) {
      reader.read(data, function(err, frame) {
        vorbisDecoder.decode(frame, function(err, frame) {
          toPcmS16.resample(frame, function(err, frame) {
            queue.push(frame);
          });
        });
      });
    });
    var work = function() {
      var noFrame = true;
      while(true) {
        var frame = queue.shift();
        if(frame != null) {
          noFrame = false;
          if(!player.queue(frame)) {
            queue.unshift(frame);
            break;
          }
        }
        else {
          break;
        }
      }
      if(noFrame)
        counter ++;
      else
        counter = 0;
      if(counter > 5) {
        resolve();
        return;
      }
      setTimeout(work, 10);
    };
    setTimeout(work, 1000);
  });
}

start()
//.then(h264DecodeTest)
//.then(h265DecodeTest)
//.then(vp8DecodeTest)
//.then(vp9DecodeTest)
//.then(mjpegDecodeTest)
//.then(theoraDecodeTest)
.then(aacDecodeTest)
.then(mp3DecodeTest)
.then(adpcmimawavDecodeTest)
.then(opusDecodeTest)
.then(speexDecodeTest)
.then(vorbisDecodeTest)
.then(function() {
  console.log("all done.");
});