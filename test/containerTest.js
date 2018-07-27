/**
 * コンテナ読み書きの動作についてしらべるテストコード
 */
var tt = require("../");
var fs = require("fs");
var assert = require('assert');

// 読み込みテストは、コンテナのデータから読み込みが実行できればOK
// 読み込んでなんらかのフレームがcallbackされることを期待する
var readerTest = (
    inputFile,
    reader,
    done) => {
  var message = "";
  var count = 0;
  var readableStream = fs.createReadStream(inputFile);
  readableStream.on("data", (data) => {
    if(!reader.readFrame(data, (frame) => {
      count ++;
      if(count > 10) {
        readableStream.close();
      }
    return true;
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
      assert.fail("フレームが読み込まれませんでした")
    }
    else {
      assert.ok(count);
    }
    done();
  });
};

// 書き出しテストは実際に動作させて、該当のデータを書き出してみる。
// 書き出しデータがある程度できればテストはパス
// あと余力があれば、書き出したデータをVLCとかで再生してみればよい
var writerTest = (
    inputFile,
    outputFile,
    reader,
    writer,
    done) => {
  var message = "";
  var count = 0;
  var readableStream = fs.createReadStream(inputFile);
  var writableStream = fs.createWriteStream(outputFile);
  readableStream.on("data", (data) => {
    if(!reader.readFrame(data, (frame) => {
      return writer.writeFrame(frame, (data) => {
        count ++;
        writableStream.write(data);
        if(count > 10) {
          readableStream.close();
        }
        return true;
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
      assert.fail("フレームが処理できませんでした");
    }
    else {
      assert.ok(count);
    }
    done();
  });
};

describe('reader', () => {
  describe('flv', () => {
    it('h264 / aac', (done) => {
      readerTest(
        process.env.HOME + "/tools/data/source/test.h264.aac.flv",
        new tt.reader.FlvReader(),
        done);
    });
    it('flv1 / nellymoser', (done) => {
      readerTest(
        process.env.HOME + "/tools/data/source/test.flv1.nellymoser.flv",
        new tt.reader.FlvReader(),
        done);
    });
  });
  describe('mkv', function () {
    it('png', (done) => {
      readerTest(
        process.env.HOME + "/tools/data/source/test.png.mkv",
        new tt.reader.MkvReader(),
        done);
    });
    it('h264 / aac', (done) => {
      readerTest(
        process.env.HOME + "/tools/data/source/test.h264.aac.mkv",
        new tt.reader.MkvReader(),
        done);
    });
  });
});

describe('writer', (suite) => {
  describe('flv', function () {
    it('h264 / aac', (done) => {
      writerTest(
        process.env.HOME + "/tools/data/source/test.h264.aac.flv",
        process.env.HOME + "/tools/data/node_out/test.h264.aac.flv",        
        new tt.reader.FlvReader(),
        new tt.writer.FlvWriter("h264", "aac"),
        done);
    });
  });
});