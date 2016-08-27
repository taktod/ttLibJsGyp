console.log("avcodecの動作テスト speex");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var decoder = require("../../").decoder;
var avMp3Decoder = new decoder.AvcodecAudio(16000, 1, "speex");
var encoder = require("../../").encoder;
var mp3Encoder = new encoder.Mp3lame(16000, 1, 8); // mp3って16kHzできたっけ？
var resampler = require("../../").resampler;
var audioResampler = new resampler.Audio("pcmS16", "littleEndian");

var readableStream = fs.createReadStream("smile.vp6.speex.flv");
var writableStream = fs.createWriteStream("smile.speex.mp3");
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        if(err != null) {
            console.log(err);
        }
        else {
            console.log(frame);
/*            avMp3Decoder.decode(frame, function(err, frame) {
                if(err != null) {
                    console.log(err);
                }
                else {
                    audioResampler.resample(frame, function(err, frame) {
                        mp3Encoder.encode(frame, function(err, frame) {
                            console.log(frame);
                            if(frame != null) {
                                writableStream.write(frame["data"]);
                            }
                        });
                    });
                }
            });*/
        }
    });
});
