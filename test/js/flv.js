// flvデータの動作テスト
console.log("flv絡みの動作テスト");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var flvWriter = new flv.Writer("h264", "aac");
var readableStream = fs.createReadStream("mario.flv");
var writableStream = fs.createWriteStream("mario_out.flv");
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        console.log(frame);
        flvWriter.write(frame, function(err, data) {
            writableStream.write(data);
        });
    });
});
