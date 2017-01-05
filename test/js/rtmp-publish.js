var net = require("../../").net;
var rtmpClient = net.RtmpClient;

var fs = require("fs");
var readableStream = fs.createReadStream(process.env["HOME"] + "/tools/data/source/test.h264.aac.mkv");

var mkv = require('../../').mkv;
var mkvReader = new mkv.Reader();

var nc = new rtmpClient.NetConnection();
var ns = null;
var frames = [];
var startTime = 0;
nc.addEventListener("onStatusEvent", function(event) {
    console.log(event);
    switch(event.info.code) {
    case 'NetConnection.Connect.Success':
        // var ns = new rtmpClient.NetStream(nc);と書いちゃだめ。
        // このeventListenerを抜けたらnsがgcの回収対象になってしまうため、メモリーリークが発生する
        ns = new rtmpClient.NetStream(nc);
        ns.publish("test");
        ns.addEventListener("onStatusEvent", function(event) {
            if(event.info.code == "NetStream.Publish.Start") {
                console.log("publish開始できるようになった。");
                // ここから先はframeをなんとかして取得してデータを送りつければよい。
                readableStream.on('data', function(data) {
                    // 一旦frameを解析したら、配列にいれとく。
                    mkvReader.read(data, function(err, frame) {
                        frames.push(frame);
                    });
                });
                startTime = new Date().getTime();
                // intervalで時間の進み具合と見比べながら、データをサーバーに送りつけていく。
                setInterval(function() {
                    var currentTime = new Date().getTime() - startTime;
                    while(frames.length > 0) {
                        var frame = frames.shift();
                        ns.queueFrame(frame);
                        if(frame.pts > currentTime) {
                            break;
                        }
                    }
                }, 100);
            }
        });
        break;
    default:
        break;
    }
});
nc.connect("rtmp://localhost/live");
