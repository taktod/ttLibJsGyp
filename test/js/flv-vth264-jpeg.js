// openh264Decodeの動作テスト
console.log("vth264Decodeの動作テスト jpegにしてみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var decoder = require("../../").decoder;
var vth264Decoder = new decoder.VtDecompressSession("h264");
var encoder = require("../../").encoder;
var jpegEncoder = new encoder.Jpeg(640, 360, 90);
var readableStream = fs.createReadStream("mario.flv");
var counter = 0;
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        vth264Decoder.decode(frame, function(err, frame) {
            jpegEncoder.encode(frame, function(err, frame) {
                if(frame == null) {
                    return;
                }
                console.log(frame);
                counter ++;
                fs.writeFile("jpeg/mario2_" + counter + ".jpeg", frame["data"]);
            });
        });
    });
});
