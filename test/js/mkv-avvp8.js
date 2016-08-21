// mkvデータの動作テスト
console.log("mkv絡みの動作テスト");
var fs = require('fs');
var mkv = require('../../').mkv;
var mkvReader = new mkv.Reader();
var decoder = require("../../").decoder;
var avcodecVideoDecoder = new decoder.AvcodecVideo(640, 360, "vp8");
var readableStream = fs.createReadStream("mario.webm");
readableStream.on('data', function(data) {
    mkvReader.read(data, function(err, frame) {
        if(err != null) {
            console.log(err);
        }
        else {
            if(frame["type"] == "vp8") {
                avcodecVideoDecoder.decode(frame, function(err, frame) {
                    if(err != null) {
                        console.log(err);
                    }
                    else {
                        console.log(frame);
                    }
                });
            }
        }
    });
});
