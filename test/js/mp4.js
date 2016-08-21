// mp4データの動作テスト
console.log("mp4絡みの動作テスト");
var fs = require('fs');
var mp4 = require('../../').mp4;
var mp4Reader = new mp4.Reader();
var readableStream = fs.createReadStream("mario.mp4");
readableStream.on('data', function(data) {
    mp4Reader.read(data, function(err, frame) {
        console.log(frame);
    });
});
