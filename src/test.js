console.log("oot");

var test = require('bindings')("test").testFunc();

/*
console.log(test);
test.testFunc("123");
console.log(test.testFunc);
var res = test.testFunc();
res.hoge = "123";
res.func();
console.log(res);
console.log("here");*/

/*
var flvReader = require('ttLibJsGyp').flvReader(); // flvReaderのオブジェクトを応答する。
// こんな書き方が理想かな。
var tt = {};
tt.flvReader = require('ttLibJsGyp').flvReader;
var reader = new tt.FlvReader();
var b = new ArrayBuffer();
// binaryからデータを読み込んでflvとしてcallbackする。
// frameが応答されているとしたとしても、objectとして扱えるようにしておけばよさそうではあるが・・・
reader.read(b, function(flv) { // これ・・・frameではなく、オブジェクトを応答する必要はあるのだろうか・・・
    // そもそもcontainerオブジェクトを応答するのは、コンテナオブジェクトから情報を抽出できるかもしれない
    flv.getFrame(function(frame) {
        // このframeがh264だったりaacだったりする。
        // という具合かな。
    });
});
// fs.readStream
とりあえずbinaryデータを前から順にいれていったら、なにかするというプログラムをつくっておこう。
いまのところすぐにほしいものは、mkvReader、これがほしい。
あとはwebSocketからの応答を読み込むもの。
これもほしい。
パーサーはいらないか・・・webSocketで相手にデータを送りつけて、あとは適当に再生してもらえればそれでよしっぽいし。
とりあえずフレームの取り出しができないと話しにならないから、とり出せるように仕向けたい。
あとフレームの扱い方法もきまってないと話しにならないから、そこも決めておきたい。
等にフレームの扱い方法はいろいろと使い回すと思われるので、うまくやっておかないと・・・
複数ソースでうまくやるには・・・的なものが必要だな。
*/
