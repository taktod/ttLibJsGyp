// mkvデータの動作テスト
console.log("mkv絡みの動作テスト");
var fs = require('fs');
var mkv = require('../../').mkv;
var mkvReader = new mkv.Reader();
var mkvWriter = new mkv.Writer("vp8", "opus");
var readableStream = fs.createReadStream("mario.webm");
var writableStream = fs.createWriteStream("mario_out.webm");
readableStream.on('data', function(data) {
    mkvReader.read(data, function(err, frame) {
        console.log(frame);
        mkvWriter.write(frame, function(err, data) {
            writableStream.write(data);
        });
    });
});
