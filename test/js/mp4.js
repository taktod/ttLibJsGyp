// mp4データの動作テスト
console.log("mp4絡みの動作テスト");
var fs = require('fs');
var mp4 = require('../../').mp4;
var mp4Reader = new mp4.Reader();
var mp4Writer = new mp4.Writer("h264", "aac");
mp4Writer.enableDts = true;
mp4Writer.splitType = mp4Writer.splitTypeInner;
var readableStream = fs.createReadStream("mario.mp4");
var writableStream = fs.createWriteStream("mario_out.mp4");
readableStream.on('data', function(data) {
    mp4Reader.read(data, function(err, frame) {
        //console.log(frame);
        console.log(mp4Writer);
        mp4Writer.write(frame, function(err, data) {
            writableStream.write(data);
        });
    });
});
