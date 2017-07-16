var tt = require("..");
tt.MsSetup.CoInitialize();
var loopback = new tt.MsLoopback("Japanese", "スピーカー (Realtek High Definition Audio)");

var writer = new tt.writer.MkvWriter(1000, "pcmS16");
var encoder = new tt.encoder.MSAacEncoder(48000, 2, 96000);

//var writableStream = require("fs").createWriteStream("output2.mkv");

setInterval(() => {
  loopback.queryFrame((err, frame) => {
    frame.id = 1;
    console.log("frameできた");
    return encoder.encode(frame, (err, frame) => {
      console.log(frame);
      return true;
    });
/*    return writer.writeFrame(frame, (err, data) => {
      writableStream.write(data);
      return true;
    });*/
  });
}, 500);

// 終了時にtt.MsSetup.CoUninitialize();したいけど・・・