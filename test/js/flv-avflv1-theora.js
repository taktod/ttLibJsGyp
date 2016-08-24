// openh264Decodeの動作テスト
console.log("openh264Decodeの動作テスト jpegにしてみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
console.log(flvReader);
var decoder = require("../../").decoder;
var flv1Decoder = new decoder.AvcodecVideo(320, 240, "flv1");
var encoder = require("../../").encoder;
var theoraEncoder = new encoder.Theora(320, 240);
var theoraDecoder = new decoder.Theora();
var readableStream = fs.createReadStream("mario.flv1.mp3.flv");
var counter = 0;
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        flv1Decoder.decode(frame, function(err, frame) {
            if(err) {
                console.log(err);
                return;
            }
            theoraEncoder.encode(frame, function(err, frame){
                if(err) {
                    console.log(err);
                    return;
                }
//                console.log(frame);
                theoraDecoder.decode(frame, function(err, frame) {
                    if(err) {
                        console.log(err);
                        return;
                    }
                    console.log(frame);
                });
            });
        });
    });
});
