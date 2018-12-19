/**
 * 画像データをいじる動作テスト
 */
var tt = require("../");
var fs = require("fs");
var assert = require("assert");

describe("imageCheck", () => {
  it("jpeg", (done) => {
    // binaryからpngデータを復元するテスト処理
    var frame = new tt.Frame();
    frame.binary = fs.readFileSync("./test/target.jpeg");
    frame.type = "jpeg";
    frame.timebase = 1000;
    frame.pts = 0;
    frame.restore();
    var decoder = new tt.decoder.AvcodecVideoDecoder(frame.type, frame.width, frame.height);
    decoder.decode(frame, (frame) => {
      var encoder = new tt.encoder.JpegEncoder(frame.width, frame.height, 90);
      return encoder.encode(frame, (frame) => {
        fs.writeFileSync("output_jpeg.jpeg", frame.getBinaryBuffer());
        done();
        return true;
      });
    });
  });
  it("png", (done) => {
    var frame = new tt.Frame();
    frame.binary = fs.readFileSync("./test/target.png");
    frame.type = "png";
    frame.timebase = 1000;
    frame.pts = 0;
    frame.restore();
    var decoder = new tt.decoder.AvcodecVideoDecoder(frame.type, frame.width, frame.height);
    decoder.decode(frame, (frame) => {
      var resampler = new tt.resampler.SwscaleResampler(
        frame.type, frame.subType, frame.width, frame.height,
        "yuv", "planar", frame.width, frame.height,
        "bilinear");
      return resampler.resample(frame, (frame) => {
        var encoder = new tt.encoder.JpegEncoder(frame.width, frame.height, 90);
        return encoder.encode(frame, (frame) => {
          fs.writeFileSync("output_png.jpeg", frame.getBinaryBuffer());
          done();
          return true;
        });
      });
    });
  });
  it("libpng", (done) => {
    var frame = new tt.Frame();
    frame.binary = fs.readFileSync("./test/target.png");
    frame.type = "png";
    frame.timebase = 1000;
    frame.pts = 0;
    frame.restore();
    var decoder = new tt.decoder.PngDecoder(frame.type, frame.width, frame.height);
    decoder.decode(frame, (frame) => {
      var resampler = new tt.resampler.SwscaleResampler(
        frame.type, frame.subType, frame.width, frame.height,
        "yuv", "planar", frame.width, frame.height,
        "bilinear");
      return resampler.resample(frame, (frame) => {
        var encoder = new tt.encoder.JpegEncoder(frame.width, frame.height, 90);
        return encoder.encode(frame, (frame) => {
          fs.writeFileSync("output_libpng.jpeg", frame.getBinaryBuffer());
          done();
          return true;
        });
      });
    });
  });
});
