// audioConverterによるaacのdecode動作テストそのまま再生
console.log("acAacDecodeの動作テスト 再生してみよう。");
var fs = require('fs');
var flv = require('../../').flv;
var flvReader = new flv.Reader();
var decoder = require("../../").decoder;
var acAacDecoder = new decoder.AudioConverter(44100, 2, "mp3");
var util = require("../../").util;
var auPlayer = new util.AudioUnitPlayer(44100, 2);

var readableStream = fs.createReadStream("smile.vp6.mp3.flv");
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        acAacDecoder.decode(frame, function(err, frame) {
            console.log(frame);
            auPlayer.queue(frame);
        });
    });
});
