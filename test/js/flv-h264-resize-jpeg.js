// openh264Decodeの動作テスト
console.log("openh264Decodeの動作テスト jpegにしてみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var decoder = require("../../").decoder;
var openh264Decoder = new decoder.Openh264();
var encoder = require("../../").encoder;
var jpegEncoder = new encoder.Jpeg(320, 180, 90);
var resampler = require("../../").resampler;
var imageResizer = new resampler.YuvImageResizer("planar", 320, 180, false);
console.log(imageResizer);
var readableStream = fs.createReadStream("mario.flv");
var counter = 0;
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        if(err) return;
        openh264Decoder.decode(frame, function(err, frame) {
            if(err) return;
            imageResizer.resample(frame, function(err, frame) {
                if(err) return;
                jpegEncoder.encode(frame, function(err, frame) {
                    if(frame == null) {
                        return;
                    }
                    counter ++;
                    fs.writeFile("jpeg/mario_" + counter + ".jpeg", frame["data"]);
                });
            });
        });
    });
});
