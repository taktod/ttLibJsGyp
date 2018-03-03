var tt = require("../");
var fs = require("fs");
var reader = new tt.reader.WebmReader();
//var writer = new tt.writer.WebmWriter(1000, "vp8", "vorbis");
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.vp9.opus.webm");
var opusDecoder = new tt.decoder.OpusDecoder(48000, 2);
var opusEncoder = new tt.encoder.OpusEncoder(48000, 2, 480);
console.log(opusEncoder.getCodecControl("OPUS_GET_BITRATE"));
opusEncoder.setCodecControl("OPUS_SET_BITRATE", 96000);
console.log(opusEncoder.getCodecControl("OPUS_GET_BITRATE"));

readableStream.on("data", (data) => {
  if(!reader.readFrame(data, (frame) => {
    if(frame.type == "opus") {
      return opusDecoder.decode(frame, (pcm) => {
        return opusEncoder.encode(pcm, (opus) => {
//          console.log(opus);
          return true;
        });
      });
    }
    return true;
  })) {
    console.log("処理エラー発生");
    readableStream.close();
  }
});