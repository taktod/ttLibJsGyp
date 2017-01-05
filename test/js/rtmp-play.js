var net = require("../../").net;
var rtmpClient = net.RtmpClient;

var nc = new rtmpClient.NetConnection();
var ns = null;
nc.addEventListener("onStatusEvent", function(event) {
    console.log(event);
    switch(event.info.code) {
    case 'NetConnection.Connect.Success':
        // var ns = new rtmpClient.NetStream(nc);と書いちゃだめ。
        // このeventListenerを抜けたらnsがgcの回収対象になってしまうため、メモリーリークが発生する
        ns = new rtmpClient.NetStream(nc);
        ns.setBufferLength(2);
        ns.addEventListener("onStatusEvent", function(event) {
            console.log(event);
        });
        ns.setFrameListener(function(err, frame) {
            // play時にフレームを取得したときにcallbackされます。
            if(err) {
                console.log(err);
                return;
            }
            console.log(frame.type + " " + frame.pts + " / " + frame.timebase);
        });
        // testで放送 videoあり audioあり
        ns.play("test");
        // あとはほっといてもplayのデータが送られてくる的な感じ。
        break;
    default:
        break;
    }
});
nc.connect("rtmp://localhost/live");
