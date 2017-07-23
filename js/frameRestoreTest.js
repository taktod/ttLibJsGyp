var tt = require("..");

var encoder = new tt.encoder.JpegEncoder(320, 240, 95);

var frame = new tt.Frame();
frame.binary = new Uint8Array(115200);
frame.id = 1;
frame.type = "yuv";
frame.pts = 0;
frame.timebase = 1000;
frame.subType = "planar";
frame.width = 320;
frame.height = 240;
frame.yStride = 320;
frame.uStride = 160;
frame.vStride = 160;

frame.restore();
console.log(frame);

encoder.encode(frame, (frame) => {
  console.log(frame);
  return true;
});