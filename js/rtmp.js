var tt = require("../");
var fs = require("fs");

var nc = new tt.rtmp.NetConnection();
var ns = null;
// 視聴動作の場合
nc.on("onStatusEvent", (event) => {
  console.log(event);
  switch(event.info.code) {
  case "NetConnection.Connect.Success":
    ns = new tt.rtmp.NetStream(nc);
    ns.setBufferLength(2);
    ns.on("onStatusEvent", (event) => {
      console.log(event);
    });
    ns.on("onFrameCallback", (err, frame) => {
      if(frame.type == "aac") {
        console.log(frame.type + " " + frame.pts);
      }
    });
    ns.play("test");
    break;
  default:
    break;
  }
});
/*
// 配信動作の場合
var reader = new tt.reader.FlvReader();
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.flv");

var frames = [];
var startTime = 0;
nc.on("onStatusEvent", (event) => {
  switch(event.info.code) {
  case "NetConnection.Connect.Success":
    console.log("コネクト作成した。");
    ns = new tt.rtmp.NetStream(nc);
    ns.on("onStatusEvent", (event) => {
      console.log(event.info.code);
      if(event.info.code == "NetStream.Publish.Start") {
        startTime = new Date().getTime();
        setInterval(() => {
          var currentTime = new Date().getTime() - startTime;
          console.log(currentTime);
          while(frames.length > 0) {
            var frame = frames.shift();
            ns.queueFrame(frame);
            if(frame.pts > currentTime) {
              break;
            }
          }
        }, 1000);
        readableStream.on("data", (data) => {
          reader.readFrame(data, (err, frame) => {
            frames.push(frame.clone());
            return true;
          });
        });
      }
    });
    ns.publish("test");
    break;
  default:
    break;
  }
});
*/
nc.connect("rtmp://rtmpTestServer.com/live");
