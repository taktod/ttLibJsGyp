// mkvのデータを取得後、opusの部分のみ取り出し、pcmに変換する動作 さらにaac(adts)にしよう。
console.log("mkvからopusを取り出してdecodeしてみる。");
var fs = require("fs");
var mkv = require("../../").mkv;
var mkvReader = new mkv.Reader();
var decoder = require("../../").decoder;
var encoder = require("../../").encoder;
var opusDecoder = new decoder.Opus(48000, 2);
var aacEncoder = new encoder.Faac("low", 48000, 2, 96000);
var readableStream = fs.createReadStream("mario.webm");
var writableStream = fs.createWriteStream("mario.aac");
readableStream.on("data", function(data) {
    mkvReader.read(data, function(err, frame) {
        opusDecoder.decode(frame, function(err, frame) {
            aacEncoder.encode(frame, function(err, frame) {
                console.log(frame);
                if(frame != null) {
                    // このデータを書き出す。
                    writableStream.write(frame["data"]);
                }
            });
        });
    });
});
