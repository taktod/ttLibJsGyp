// mkvデータの動作テスト
console.log("mkv絡みの動作テスト");
var fs = require('fs');
var mkv = require('../../').mkv;
var mkvReader = new mkv.Reader();
var readableStream = fs.createReadStream("/Users/taktod/tools/data/source/test.h265.mp3.mkv");
readableStream.on('data', function(data) {
    mkvReader.read(data, function(err, frame) {
        if(frame.type == "h265") {
            console.log((frame.pts * 16)+ " " + frame.data.length + " " + frame.timebase);
        }
    });
});
