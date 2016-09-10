// audioConverterによるaacのdecode動作テスト
console.log("acAacDecodeの動作テスト mp3にしてみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var decoder = require("../../").decoder;
var avAacDecoder = new decoder.AvcodecAudio(44100, 2, "aac");
var encoder = require("../../").encoder;
var opusEncoder = new encoder.Opus(48000, 2, 480);
var resampler = require("../../").resampler;
var audioResampler = new resampler.Audio("pcmS16", "littleEndian");
var dspResampler = new resampler.Speexdsp(2, 44100, 48000, 8);

var readableStream = fs.createReadStream("mario.flv");
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        if(err != null) {
            console.log(err);
            return;
        }
        avAacDecoder.decode(frame, function(err, frame) {
            if(err != null) {
                console.log(err);
                return;
            }
            audioResampler.resample(frame, function(err, frame) {
                if(err) {
                    console.log(err);
                    return;
                }
                dspResampler.resample(frame, function(err, frame) {
                    if(err) {
                        console.log(err);
                        return;
                    }
                    opusEncoder.encode(frame, function(err, frame) {
                        if(err) {
                            console.log(err);
                            return;
                        }
                        console.log(frame.pts);
                    });
                });
            });
        });
    });
});
