console.log("avcodecの動作テスト mp3をデコード");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
console.log(flvReader);
var decoder = require("../../").decoder;
var mp3Decoder = new decoder.AvcodecAudio(44100, 2, "mp3");
var resampler = require("../../").resampler;
var pcmResampler = new resampler.Audio("pcmS16", "littleEndian");
var readableStream = fs.createReadStream("mario.flv1.mp3.flv");
var counter = 0;
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        if(err) {
            console.log(err);
            return;
        }
        mp3Decoder.decode(frame, function(err, frame) {
            if(err) {
                console.log(err);
                return;
            }
            pcmResampler.resample(frame, function(err, frame) {
                if(err) {
                    console.log(err);
                    return;
                }
                console.log(frame);
            });
        });
    });
});
