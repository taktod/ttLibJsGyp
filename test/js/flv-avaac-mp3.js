// audioConverterによるaacのdecode動作テスト
console.log("acAacDecodeの動作テスト mp3にしてみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var decoder = require("../../").decoder;
var avAacDecoder = new decoder.AvcodecAudio(44100, 2, "aac");
var encoder = require("../../").encoder;
var mp3Encoder = new encoder.Mp3lame(44100, 2, 8);
var resampler = require("../../").resampler;
var audioResampler = new resampler.Audio("pcmS16", "littleEndian");

var readableStream = fs.createReadStream("mario.flv");
var writableStream = fs.createWriteStream("mario_avaac.mp3");
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        if(err != null) {
            console.log(err);
        }
        else {
            avAacDecoder.decode(frame, function(err, frame) {
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
            });
        }
    });
});
