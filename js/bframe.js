// bframe関連を利用した動作のテスト

// x264でbframe入りのmain profileのh264をつくって、rtmpに流してみるとどうなるかやってみる。
var tt = require("../");
var fs = require("fs");

var reader = new tt.reader.FlvReader();
var writer = new tt.writer.FlvWriter("h264", "aac");
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.flv");
//var writableStream = fs.createWriteStream("output.flv");

var videoDecoder = new tt.decoder.AvcodecVideoDecoder("h264", 640, 360);
var x264Encoder = new tt.encoder.X264Encoder(
  640,
  360,
  "veryfast",
  "zerolatency",
  "main",
  JSON.parse("{\n\"open-gop\":1,\n\"threads\":1,\n\"merange\":16,\n\"qcomp\":0.6,\n\"ip-factor\": 0.71,\n\"bitrate\":300,\n\"qp\":21,\n\"crf\":23,\n\"crf-max\":23,\n\"fps\":\"30/1\",\n\"keyint\":150,\n\"keyint-min\":150,\n\"bframes\":3,\n\"vbv-maxrate\":0,\n\"vbv-bufsize\":1024,\n\"qp-max\":40,\n\"qp-min\": 21,\n\"qp-step\": 4\n}"));

// rtmpに流す場合の処理
var frames = [];
var startTime = 0;
var nc = new tt.rtmp.NetConnection();

nc.on("onStatusEvent", (event) => {
  switch(event.info.code) {
  case "NetConnection.Connect.Success":
    ns = new tt.rtmp.NetStream(nc);
    ns.on("onStatusEvent", (event) => {
      console.log(event.info.code);
      if(event.info.code == "NetStream.Publish.Start") {
        startTime = new Date().getTime();
        setInterval(() => {
          var currentTime = new Date().getTime() - startTime;
          while(frames.length > 0) {
            var frame = frames.shift();
            ns.queueFrame(frame);
            if(frame.pts > currentTime) {
              break;
            }
          }
        }, 1000);
        readableStream.on("data", (data) => {
  if(!reader.readFrame(data, (err, frame) => {
    if(frame.type == "h264") {
      return videoDecoder.decode(frame, (err, frame) => {
        return x264Encoder.encode(frame, (err, frame) => {
          frame.id = 9;
          frames.push(frame.clone());
          return true;
        });
      });
    }
    else {
      frames.push(frame.clone());
      return true;
    }
  })) {
    console.log("readFrame失敗");
    readableStream.close();
  }
        });
      }
    });
    ns.publish("test");
    break;
  default:
    console.log(event.info.code);
    break;
  }
});
nc.connect("rtmp://rtmpTestServer.com/live");
/*
// ファイルに書き出す場合の処理

var writeFrame = (frame) => {
  return writer.writeFrame(frame, (err, data) => {
    writableStream.write(data);
    return true;
  });
}

readableStream.on("data", (data) => {
  if(!reader.readFrame(data, (err, frame) => {
    if(frame.type == "h264") {
      return videoDecoder.decode(frame, (err, frame) => {
        return x264Encoder.encode(frame, (err, frame) => {
          frame.id = 9;
          return writeFrame(frame);
        });
      });
    }
    else {
      return writeFrame(frame);
    }
  })) {
    console.log("readFrame失敗");
    readableStream.close();
  }
});

// */