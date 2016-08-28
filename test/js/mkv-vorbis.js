// mkvのデータを取得後、vorbisの部分のみ取り出し、pcmに変換する動作 さらにmp3にしよう。
console.log("mkvからvorbisを取り出してdecodeしてみる。");
var fs = require("fs");
var mkv = require("../../").mkv;
var mkvReader = new mkv.Reader();
var decoder = require("../../").decoder;
var encoder = require("../../").encoder;
var resampler = require("../../").resampler;
var vorbisDecoder = new decoder.Vorbis();
var mp3Encoder = new encoder.Mp3lame(44100, 2, 8);
var audioResampler = new resampler.Audio("pcmS16", "littleEndian");
var readableStream = fs.createReadStream("mario.theora.vorbis.mkv");
var writableStream = fs.createWriteStream("mario.vorbis.mp3");
readableStream.on("data", function(data) {
    mkvReader.read(data, function(err, frame) {
        if(err) {
            console.log(err);
            return;
        }
        vorbisDecoder.decode(frame, function(err, frame) {
            if(err) {
                console.log(err);
                return;
            }
            audioResampler.resample(frame, function(err, frame) {
                if(err) {
                    console.log(err);
                    return;
                }
                mp3Encoder.encode(frame, function(err, frame) {
                    if(err) {
                        console.log(err);
                        return;
                    }
                    writableStream.write(frame["data"]);
                });
            });
        });
    });
});
