/**
 * コーデックによる変換動作について
 */
var tt = require("../");
var fs = require("fs");
var assert = require('assert');

// decode動作は適当なデータにして、結果を確認できるようにしておきたい。
var decoderVideoTest = (
    inputName,
    type,
    reader,
    decoder,
    outputName,
    done) => {
  var message = "";
  var count = 0;
  var readableStream = fs.createReadStream(inputName);
  var writableStream = fs.createWriteStream(outputName + ".flv");
  var resampler = {name:"empty", resample:(frame, func) => {return func(frame);}};
  var encoder = {name:"empty", encode:(frame, func) => {return func(frame);}};
  var writer = new tt.writer.FlvWriter("h264", "");
  readableStream.on("data", (data) => {
    if(!reader.readFrame(data, (frame) => {
      if(frame.type != type) {
        return true;
      }
      return decoder.decode(frame, (frame) => {
        if(resampler.name == "empty" && (frame.type != "yuv" || frame.subType != "planar")) {
          resampler = new tt.resampler.SwscaleResampler(
            frame.type, frame.subType, frame.width, frame.height,
            "yuv", "planar", frame.width, frame.height, "Bilinear");
        }
        return resampler.resample(frame, (frame) => {
          if(encoder.name == "empty") {
            encoder = new tt.encoder.X264Encoder(
                frame.width,
                frame.height,
                "",
                "",
                "",
                {});
          }
          return encoder.encode(frame, (frame) => {
            // これを適当なデータにする。
            if(frame.type != "h264") {
              return false;
            }
            frame.id = 9;
            return writer.writeFrame(frame, (data) => {
              count ++;
              writableStream.write(data);
              if(count > 10) {
                readableStream.close();
              }
              return true;
            });
          });
        });
      });
    })) {
      message = "処理エラーが発生しました";
      readableStream.close();
    }
  });
  readableStream.on("close", () => {
    if(message != "") {
      assert.fail(message);
    }
    else if(count == 0) {
      assert.fail("フレームがデコードされませんでした");
    }
    else {
      assert.ok(count);
    }
    done();
  });
};

describe("decoder", () => {
  describe("avcodec", () => {
    it("h264", (done) => {
      decoderVideoTest(
        process.env.HOME + "/tools/data/source/test.h264.aac.mkv",
        "h264",
        new tt.reader.MkvReader(),
        new tt.decoder.AvcodecVideoDecoder("h264", 640, 360),
        "test/decoder.avcodec.h264",
        done);
    });
    it("png", (done) => {
      decoderVideoTest(
        process.env.HOME + "/tools/data/source/test.png.mkv",
        "png",
        new tt.reader.MkvReader(),
        new tt.decoder.AvcodecVideoDecoder("png", 640, 360),
        "test/decoder.avcodec.png",
        done);
    });
  });
});

describe("encoder", () => {
  describe("avcodec", () => {

  });
});