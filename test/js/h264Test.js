var ttg = require("../../");
var capture = new ttg.util.OpencvCapture(0, 320, 240);
var org_win = new ttg.util.OpencvWindow("original");
var dec_win = new ttg.util.OpencvWindow("decode");
var encoder = new ttg.encoder.X264(320, 240, 40, 4, 500000, 3, "veryfast", "zerolatency", "main");
var decoder = new ttg.decoder.AvcodecVideo(320, 240, "h264");
var toYuv = new ttg.resampler.Image("yuv420", "planar");
var toBgr = new ttg.resampler.Image("bgr", "bgr");

setInterval(function() {
    // captureする。
    capture.query(function(err, frame) {
        // original window描画
        org_win.show(frame);
        // bgr -> yuv変換
        toYuv.resample(frame, function(err, frame) {
            // yuv -> h264変換
            encoder.encode(frame, function(err, frame) {
                console.log(frame);
                // h264 -> yuv変換
                decoder.decode(frame, function(err, frame) {
                    // yuv -> bgr変換
                    toBgr.resample(frame, function(err, frame) {
                        // dec_win描画
                        dec_win.show(frame);
                    });
                });
            });
        });
        // window表示(org_winもdec_winも更新される)
        org_win.update(1);
    });
}, 30);
