var ttg = require("../../");
var capture = new ttg.util.OpencvCapture(0, 320, 240);
var org_win = new ttg.util.OpencvWindow("original");
var clip_win = new ttg.util.OpencvWindow("clip");
var rsz_win = new ttg.util.OpencvWindow("resize");
var resize = new ttg.resampler.BgrImageResizer("bgr", 160, 120);

var shift = (80 + 320 * 60) * 3;
setInterval(function() {
    capture.query(function(err, frame) {
        org_win.show(frame);
        resize.resample(frame, function(err, frame) {
            rsz_win.show(frame);
        });
        // 切り抜きはこんな感じ。
        frame.width = 160;
        frame.height = 120;
        frame.data = new Buffer(new Uint8Array(frame.data).subarray(shift));
        clip_win.show(frame);
        org_win.update(1);
    });
}, 30);
