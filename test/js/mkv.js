// mkvデータの動作テスト
console.log("mkv絡みの動作テスト");
var fs = require('fs');
var mkv = require('../../').mkv;
var mkvReader = new mkv.Reader();
var mkvWriter = new mkv.Writer("h264", "aac");
var readableStream = fs.createReadStream("mario.mkv");
var writableStream = fs.createWriteStream("mario_out.mkv");
readableStream.on('data', function(data) {
    mkvReader.read(data, function(err, frame) {
        console.log(frame);
        mkvWriter.write(frame, function(err, data) {
            writableStream.write(data);
        });
    });
});
