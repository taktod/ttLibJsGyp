// avcodecの動作テスト

// とりあえずいろんなコーデックでdecodeしてみる

var tt = require("../");

var fs = require("fs");

var checkVideo = (ReaderClass, file, type) => {
  return new Promise((resolve, reject) => {
    var count = 0;
    var reader = new ReaderClass();
    var rs = fs.createReadStream(process.env.HOME + "/tools/data/source/" + file);
    var decoder = null;
    rs.on("data", (data) => {
      if(!reader.readFrame(data, (frame) => {
        if(frame.type == type) {
          if(decoder == null) {
            decoder = new tt.decoder.AvcodecVideoDecoder(frame.type, frame.width, frame.height);
          }
          return decoder.decode(frame, (frame) => {
            count ++;
            return true;
          });
        }
        return true;
      })) {
        rs.close();
        count == 0;
        return false;
      }
    });
    rs.on("end", () => {
      if(count > 0) {
        console.log(type + " ok");
        resolve();
      }
      else {
        reject();
      }
    });
  });
}

var checkAudio = (ReaderClass, file, type) => {
  return new Promise((resolve, reject) => {
    var count = 0;
    var reader = new ReaderClass();
    var rs = fs.createReadStream(process.env.HOME + "/tools/data/source/" + file);
    var decoder = null;
    rs.on("data", (data) => {
      if(!reader.readFrame(data, (frame) => {
        if(frame.type == type) {
          if(decoder == null) {
            decoder = new tt.decoder.AvcodecAudioDecoder(frame.type, frame.sampleRate, frame.channelNum);
          }
          return decoder.decode(frame, (frame) => {
            count ++;
            return true;
          });
        }
        return true;
      })) {
        rs.close();
        count == 0;
        return false;
      }
    });
    rs.on("end", () => {
      if(count > 0) {
        console.log(type + " ok");
        resolve();
      }
      else {
        reject();
      }
    });
  });
}

Promise.resolve()
// 映像 flv1 h264 h265 mjpeg screenv1 screenv2 theora vp6 vp8 vp9
//.then(() => {return checkVideo(tt.reader.FlvReader, "test.flv1.nellymoser.flv", "flv1")})
//.then(() => {return checkVideo(tt.reader.MkvReader, "test.h264.aac.mkv", "h264")})
//.then(() => {return checkVideo(tt.reader.MkvReader, "test.h265.mp3.mkv", "h265")})
//.then(() => {return checkVideo(tt.reader.Mp4Reader, "test.mjpeg.vorbis.mp4", "jpeg")})
//.then(() => {return checkVideo(tt.reader.MkvReader, "test.theora.speex.mkv", "theora")})
//.then(() => {return checkVideo(tt.reader.FlvReader, "test.screenv1.speex.flv", "screenv1")}) // *動作しない
//.then(() => {return checkVideo(tt.reader.FlvReader, "test.screenv2.pcm_alaw.flv", "screenv2")}) // *動作しない
//.then(() => {return checkVideo(tt.reader.FlvReader, "test.vp6.mp3.flv", "vp6")})
//.then(() => {return checkVideo(tt.reader.WebmReader, "test.vp8.vorbis.webm", "vp8")})
//.then(() => {return checkVideo(tt.reader.WebmReader, "test.vp9.opus.webm", "vp9")})

// 音声 aac adpcmimawav adpcmswf nellymoser pcmalaw pcmmulaw speex mp3 vorbis opus
//.then(() => {return checkAudio(tt.reader.FlvReader, "test.h264.aac.flv", "aac")})
//.then(() => {return checkAudio(tt.reader.MkvReader, "test.mjpeg.adpcmimawav.mkv", "adpcmImaWav")})
//.then(() => {return checkAudio(tt.reader.FlvReader, "test.flv1.adpcmswf.flv", "adpcmSwf")}) // *動作しない
//.then(() => {return checkAudio(tt.reader.FlvReader, "test.flv1.nellymoser.flv", "nellymoser")})
//.then(() => {return checkAudio(tt.reader.FlvReader, "test.flv1.pcm_alaw.flv", "pcmAlaw")})
//.then(() => {return checkAudio(tt.reader.FlvReader, "test.flv1.pcm_mulaw.flv", "pcmMulaw")})
//.then(() => {return checkAudio(tt.reader.FlvReader, "test.flv1.speex.flv", "speex")})
//.then(() => {return checkAudio(tt.reader.MkvReader, "test.h265.mp3.mkv", "mp3")})
//.then(() => {return checkAudio(tt.reader.WebmReader, "test.vp8.vorbis.webm", "vorbis")})
//.then(() => {return checkAudio(tt.reader.WebmReader, "test.vp9.opus.webm", "opus")})
;
