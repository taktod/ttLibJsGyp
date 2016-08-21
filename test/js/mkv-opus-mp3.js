// mkvのデータを取得後、opusの部分のみ取り出し、pcmに変換する動作 さらにmp3にしよう。
console.log("mkvからopusを取り出してdecodeしてみる。");
var fs = require("fs");
var mkv = require("../../").mkv;
var mkvReader = new mkv.Reader();
var decoder = require("../../").decoder;
var encoder = require("../../").encoder;
var opusDecoder = new decoder.Opus(48000, 2);
var mp3Encoder = new encoder.Mp3lame(48000, 2, 8);
var readableStream = fs.createReadStream("mario.webm");
var writableStream = fs.createWriteStream("mario.mp3");
readableStream.on("data", function(data) {
    mkvReader.read(data, function(err, frame) {
        opusDecoder.decode(frame, function(err, frame) {
            mp3Encoder.encode(frame, function(err, frame) {
                console.log(frame);
                if(frame != null) {
                    // このデータを書き出す。
                    writableStream.write(frame["data"]);
                }
            });
        });
    });
});
