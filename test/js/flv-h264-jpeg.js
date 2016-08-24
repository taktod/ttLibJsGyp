// openh264Decodeの動作テスト
console.log("openh264Decodeの動作テスト jpegにしてみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
console.log(flvReader);
var decoder = require("../../").decoder;
var openh264Decoder = new decoder.Openh264();
console.log(openh264Decoder);
var encoder = require("../../").encoder;
var jpegEncoder = new encoder.Jpeg(640, 360, 90);
var readableStream = fs.createReadStream("mario.flv");
var counter = 0;
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        openh264Decoder.decode(frame, function(err, frame) {
            jpegEncoder.encode(frame, function(err, frame) {
                if(frame == null) {
                    return;
                }
                console.log(frame);
                counter ++;
                fs.writeFile("jpeg/mario_" + counter + ".jpeg", frame["data"]);
            });
        });
    });
});
