// mp4データの動作テスト
console.log("mp4絡みの動作テスト");
var fs = require('fs');
var mp4 = require('../../').mp4;
var mp4Reader = new mp4.Reader();
var mp4Writer = new mp4.Writer("h264");
var readableStream = fs.createReadStream("mario.mp4");
var writableStream = fs.createWriteStream("mario_out.mp4");
readableStream.on('data', function(data) {
    mp4Reader.read(data, function(err, frame) {
        if(frame.type != "h264") {
            return;
        }
        console.log(frame);
        mp4Writer.write(frame, function(err, data) {
            writableStream.write(data);
        });
    });
});
