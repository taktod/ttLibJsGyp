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
var jpegEncoder = new encoder.Jpeg(320, 240, 90);
var readableStream = fs.createReadStream("mario.flv");
var counter = 0;
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        openh264Decoder.decode(frame, function(err, frame) {
            if(err) return;
            frame.width = 320;
            frame.height = 240;
            frame.yDataPos += 160 + 60 * frame.yStride;
            frame.uDataPos += 80 + 30 * frame.uStride;
            frame.vDataPos += 80 + 30 * frame.vStride;
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
