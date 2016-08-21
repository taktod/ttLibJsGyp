// mkvデータの動作テスト
console.log("mkv絡みの動作テスト");
var fs = require('fs');
var mkv = require('../../').mkv;
var mkvReader = new mkv.Reader();
var readableStream = fs.createReadStream("mario.mkv");
readableStream.on('data', function(data) {
    mkvReader.read(data, function(err, frame) {
        console.log(frame);
    });
});
