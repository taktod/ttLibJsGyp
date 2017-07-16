var tt = require("..");
tt.MsSetup.CoInitialize("multiThreaded");
tt.MsSetup.MFStartup();
var encoder = new tt.encoder.MSH264Encoder("H264 Encoder MF", 320, 240, 320000);
// うーん、丸Rですか・・・でもこれじゃないと、文字コードあわないんだよねぇ・・・
// 文字化けの温床にしかなさそうな気がする・・・
//var encoder = new tt.encoder.MSH264Encoder("Intel® Quick Sync Video H.264 Encoder MF", 320, 240, 320000);

// 適当なフレームをつくって、yuvとして扱えるようにしなければならない。

var yuvDummy = tt.Frame.fromBinaryBuffer(null, new Uint8Array(115200), {
  type: "yuv",
  id: 1,
  pts: 0,
  timebase: 1000,
  subType: "semiPlanar",
  width: 320,
  height: 240,
  yStride: 320,
  uStride: 320,
  vStride: 320
});
if(yuvDummy == null) {
  console.log("dummyFrameが作成できませんでした。");
}
else {
  setInterval(() => {
    encoder.encode(yuvDummy, (err, frame) => {
      console.log("encodeされました。");
      return true;
    });
    yuvDummy.pts += 100;
  }, 100);
}

// 終了時には、ちゃんとshutdownとuninitializeすべきではある。
/*
tt.MsSetup.MFShutdown();
tt.MsSetup.CoUninitialize();
*/
