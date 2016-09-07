// mkvデータの動作テスト
console.log("mkv絡みの動作テスト");
var fs = require('fs');
var mkv = require('../../').mkv;
var mkvReader = new mkv.Reader();
var mp4 = require("../../").mp4;
var mp4Writer = new mp4.Writer("h264");
var decoder = require("../../").decoder;
var encoder = require("../../").encoder;
var avcodecVideoDecoder = new decoder.AvcodecVideo(640, 360, "vp8");
var h264Encoder = new encoder.Openh264(640, 360, 40, 4, 320000);
var readableStream = fs.createReadStream("mario.webm");
var writableStream = fs.createWriteStream("mario.webm.mp4");
readableStream.on('data', function(data) {
    mkvReader.read(data, function(err, frame) {
        if(err != null) {
            console.log(err);
            return;
        }
        if(frame["type"] == "vp8") {
            avcodecVideoDecoder.decode(frame, function(err, frame) {
                if(err != null) {
                    console.log(err);
                    return;
                }
//                console.log(frame);
                h264Encoder.encode(frame, function(err, frame) {
                    if(err) {
                        console.log(err);
                        return;
                    }
                    console.log(frame);
                    frame.id = 1;
                    mp4Writer.write(frame, function(err, data) {
                        if(err) {
                            console.log(err);
                            return;
                        }
//                        console.log(data.length);
                        writableStream.write(data);
                    });
                });
            });
        }
    });
});
