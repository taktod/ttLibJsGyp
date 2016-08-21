// mpegtsデータの動作テスト
console.log("mpegts絡みの動作テスト");
var fs = require('fs');
var mpegts = require('../../').mpegts;
var mpegtsReader = new mpegts.Reader();
var mpegtsWriter = new mpegts.Writer("h264", "aac");
var readableStream = fs.createReadStream("mario.ts");
var writableStream = fs.createWriteStream("mario_out.ts");

readableStream.on('data', function(data) {
    mpegtsReader.read(data, function(err, frame) {
        console.log(frame);
        mpegtsWriter.write(frame, function(err, data) {
            writableStream.write(data);
        });
    });
});
