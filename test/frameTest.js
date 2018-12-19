var tt = require("../");

describe("frame", () => {
  describe("yuv", () => {
    it("getBinaryFrame", (done) => {
      var frame = new tt.Frame();
      frame.binary = new Uint8Array([
        1,2,3,4,5,6,7,8,9,10,11,12,
        1,2,3,4,5,6,7,8,9,10,11,12,
        1,2,3,4,5,6,7,8,9,10,11,12,
        1,2,3,4,5,6,7,8,9,10,11,12,
        1,2,3,4,5,6,7,8,9,10,11,12,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
      ]);
      frame.id = 1;
      frame.type = "yuv";
      frame.pts = 0;
      frame.timebase = 1000;
      frame.subType = "planar";
      frame.width = 10;
      frame.height = 5;
      frame.yStride = 12;
      frame.uStride = 6;
      frame.vStride = 6;
      frame.restore();
//      console.log(frame);
//      console.log(frame.getBinaryBuffer());
      if(frame.getBinaryBuffer() != null) {
        done();
      }
      else {
        console.log(frame);
        console.log(frame.getBinaryBuffer());
      }
    });
    it("clone", (done) => {
      var frame = new tt.Frame();
      frame.binary = new Uint8Array([
        1,2,3,4,5,6,7,8,9,10,11,12,
        1,2,3,4,5,6,7,8,9,10,11,12,
        1,2,3,4,5,6,7,8,9,10,11,12,
        1,2,3,4,5,6,7,8,9,10,11,12,
        1,2,3,4,5,6,7,8,9,10,11,12,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
        128,128,128,128,128,128,
      ]);
      frame.id = 1;
      frame.type = "yuv";
      frame.pts = 0;
      frame.timebase = 1000;
      frame.subType = "planar";
      frame.width = 10;
      frame.height = 5;
      frame.yStride = 12;
      frame.uStride = 6;
      frame.vStride = 6;
      frame.restore();
      var cloned = frame.clone();
      console.log(cloned);
      console.log(new Uint8Array(cloned.binary));
      if(cloned.binary != null) {
        done();
      }
      else {
        console.log(frame);
        console.log(frame.getBinaryBuffer());
      }
    });
  });
});