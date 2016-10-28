// mkvデータの動作テスト
console.log("mkvCodecの動作テスト");
var fs = require('fs');
var mkv = require('../../').mkv;
var webm = require('../../').webm;

var start = function() {
    return new Promise(function(resolve, reject) {
        resolve();
    });
}
var h264AacTest = function() {
    return new Promise(function(resolve, reject) {
        var reader = new mkv.Reader();
        var writer = new mkv.Writer("h264", "aac");
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.mkv");
        var writableStream = fs.createWriteStream(process.env.HOME + "/tools/data/node_out/test.h264.aac.mkv");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
                writer.write(frame, function(err, data) {
                    writableStream.write(data);
                });
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
var h265Mp3Test = function() {
    return new Promise(function(resolve, reject) {
        var reader = new mkv.Reader();
        var writer = new mkv.Writer("h265", "mp3");
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h265.mp3.mkv");
        var writableStream = fs.createWriteStream(process.env.HOME + "/tools/data/node_out/test.h265.mp3.mkv");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
                writer.write(frame, function(err, data) {
                    writableStream.write(data);
                });
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
var mjpegAdpcmimawavTest = function() {
    return new Promise(function(resolve, reject) {
        var reader = new mkv.Reader();
        var writer = new mkv.Writer("jpeg", "adpcmimawav");
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.mjpeg.adpcmimawav.mkv");
        var writableStream = fs.createWriteStream(process.env.HOME + "/tools/data/node_out/test.mjpeg.adpcmimawav.mkv");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
                writer.write(frame, function(err, data) {
                    writableStream.write(data);
                });
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
var theoraSpeexTest = function() {
    return new Promise(function(resolve, reject) {
        var reader = new mkv.Reader();
        var writer = new mkv.Writer("theora", "speex");
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.theora.speex.mkv");
        var writableStream = fs.createWriteStream(process.env.HOME + "/tools/data/node_out/test.theora.speex.mkv");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
                writer.write(frame, function(err, data) {
                    writableStream.write(data);
                });
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
var vp8VorbisTest = function() {
    return new Promise(function(resolve, reject) {
        var reader = new mkv.Reader();
        var writer = new webm.Writer("vp8", "vorbis");
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.vp8.vorbis.webm");
        var writableStream = fs.createWriteStream(process.env.HOME + "/tools/data/node_out/test.vp8.vorbis.webm");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
                writer.write(frame, function(err, data) {
                    writableStream.write(data);
                });
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
var vp9OpusTest = function() {
    return new Promise(function(resolve, reject) {
        var reader = new mkv.Reader();
        var writer = new webm.Writer("vp9", "opus");
        var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.vp9.opus.webm");
        var writableStream = fs.createWriteStream(process.env.HOME + "/tools/data/node_out/test.vp9.opus.webm");
        readableStream.on('data', function(data) {
            reader.read(data, function(err, frame) {
                console.log(frame.type + " " + frame.pts);
                writer.write(frame, function(err, data) {
                    writableStream.write(data);
                });
            });
        });
        readableStream.on('end', function() {
            resolve();
        });
    });
}
start()
.then(h264AacTest)
.then(h265Mp3Test)
.then(mjpegAdpcmimawavTest)
.then(theoraSpeexTest)
.then(vp8VorbisTest)
.then(vp9OpusTest)
.then(function() {
    console.log("all done.");
});
