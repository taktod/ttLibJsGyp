// mp4データの動作テスト
console.log("mp4絡みの動作テスト");
var fs = require('fs');
var mp4 = require('../../').mp4;
var start = function() {
    return new Promise(function(resolve, reject) {
        resolve();
    });
}
var h264AacTest = function() {
    return new Promise(function(resolve, reject) {
        console.log("h264 aac");
        var reader = new mp4.Reader();
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.mp4");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
var h265Mp3Test = function() {
    return new Promise(function(resolve, reject) {
        console.log("h265 mp3");
        var reader = new mp4.Reader();
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h265.mp3.mp4");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
var mjpegVorbisTest = function() {
    return new Promise(function(resolve, reject) {
        console.log("mjpeg vorbis");
        var reader = new mp4.Reader();
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.mjpeg.vorbis.mp4");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
start()
//.then(h264AacTest)
.then(h265Mp3Test)
.then(mjpegVorbisTest)
.then(function() {
    console.log("all done.");
});