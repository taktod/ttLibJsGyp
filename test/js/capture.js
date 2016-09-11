var ttg = require("../../");
var capture = new ttg.util.OpencvCapture(0, 320, 240);
var window = new ttg.util.OpencvWindow("name");

setInterval(function() {
    capture.query(function(err, frame) {
        window.show(frame);
        window.update(1);
    });
}, 30);
