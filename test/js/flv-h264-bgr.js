// openh264Decodeの動作テスト
console.log("openh264Decodeの動作テスト jpegにしてみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var decoder = require("../../").decoder;
var openh264Decoder = new decoder.Openh264();
var resampler = require("../../").resampler;
var toBgr = new resampler.Image("bgr", "bgr");
var toYuv = new resampler.Image("yuv420", "planar");
var readableStream = fs.createReadStream("mario.flv");
var counter = 0;
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        openh264Decoder.decode(frame, function(err, frame) {
            if(err) return;
            toBgr.resample(frame, function(err, frame) {
                if(err) return;
                toYuv.resample(frame, function(err, frame) {
                    if(err) return;
                    console.log(frame);
                })
            });
        });
    });
});
