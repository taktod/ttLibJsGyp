var tt = require("../");
var fs = require("fs");

/*
var frame = new tt.Frame();
frame.binary; // 適当に設定
frame.yStride; // これとかも適当に設定
frame.restore(); // binaryからデータを復元して利用する。

var frame = new tt.Frame();
frame.Copy(frameA); // 他のフレームを元にcloneを作成して自身のフレームとする。
*/
// こんな動作が必要かな。
// 利用する前に内部でrestoreしてから、c言語のフレームを確定しないといけない。

var reader = new tt.reader.FlvReader();
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.flv");

var audioFrame = new tt.Frame();
var videoFrame = new tt.Frame();

readableStream.on("data", (data) => {
  if(!reader.readFrame(data, (frame) => {
    if(frame.id == 8) {
      audioFrame.copy(frame);
    }
    else if(frame.id == 9) {
      videoFrame.copy(frame);
      console.log(videoFrame);
    }
    return true;
  })) {
    console.log("readFrame失敗");
    readableStream.close();
  }
});