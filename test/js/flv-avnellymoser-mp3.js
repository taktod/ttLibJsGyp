console.log("avcodecの動作テスト nellymoserをデコードして、mp3にする");
var fs = require('fs');
var flv = require('../../').flv;
var decoder = require("../../").decoder;
var encoder = require("../../").encoder;
var resampler = require("../../").resampler;

var flvReader = new flv.Reader();
var avNellymoserDecoder = new decoder.AvcodecAudio(8000, 1, "nellymoser");
var mp3Encoder = new encoder.Mp3lame(8000, 1, 8);
var audioResampler = new resampler.Audio("pcmS16", "littleEndian");

//var readableStream = fs.createReadStream("smile.vp6.nelly44.flv");
//var readableStream = fs.createReadStream("smile.vp6.nelly22.flv");
//var readableStream = fs.createReadStream("smile.vp6.nelly16.flv");
var readableStream = fs.createReadStream("smile.vp6.nelly8.flv");
//var writableStream = fs.createWriteStream("smile.nelly44.mp3");
//var writableStream = fs.createWriteStream("smile.nelly22.mp3");
//var writableStream = fs.createWriteStream("smile.nelly16.mp3");
var writableStream = fs.createWriteStream("smile.nelly8.mp3");
readableStream.on('data', function(data) {
    flvReader.read(data, function(err, frame) {
        if(err != null) {
            console.log(err);
        }
        else {
            console.log(frame);
            avNellymoserDecoder.decode(frame, function(err, frame) {
                if(err != null) {
                    console.log(err);
                }
                else {
                    audioResampler.resample(frame, function(err, frame) {
                        mp3Encoder.encode(frame, function(err, frame) {
                            console.log(frame);
                            if(frame != null) {
                                writableStream.write(frame["data"]);
                            }
                        });
                    });
                }
            });
        }
    });
});
