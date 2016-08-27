// avCodecDecodeの動作テスト
console.log("avcodecDecodeの動作テスト vp6 => jpegにしてみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var decoder = require("../../").decoder;
var vp6Decoder = new decoder.AvcodecVideo(512, 384, "vp6");
var encoder = require("../../").encoder;
var jpegEncoder = new encoder.Jpeg(512, 384, 90);
var readableStream = fs.createReadStream("smile.vp6.mp3.flv");
var counter = 0;
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
//        console.log(frame);
        vp6Decoder.decode(frame, function(err, frame) {
            jpegEncoder.encode(frame, function(err, frame) {
                if(frame == null) {
                    return;
                }
                console.log(frame);
                counter ++;
                fs.writeFile("jpeg/smile_" + counter + ".jpeg", frame["data"]);
            });
        });
    });
});
