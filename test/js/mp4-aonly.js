// mp4データの動作テスト
console.log("mp4絡みの動作テスト");
var fs = require('fs');
var mp4 = require('../../').mp4;
var mp4Reader = new mp4.Reader();
var mp4Writer = new mp4.Writer("aac");
var readableStream = fs.createReadStream("mario.mp4");
var writableStream = fs.createWriteStream("mario_out.aac.mp4");
readableStream.on('data', function(data) {
    mp4Reader.read(data, function(err, frame) {
        if(frame.type != "aac") {
            return;
        }
        frame.id = 1;
        console.log(frame);
        mp4Writer.write(frame, function(err, data) {
            writableStream.write(data);
        });
    });
});
