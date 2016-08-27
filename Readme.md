# タイトル

ttLibJsGyp

# 作者　

taktod<poepoemix@hotmail.com>
https://twitter.com/taktod/

# 内容

nodeでc言語を利用するためのライブラリ
こっちはemscriptenとは違って小さなモジュールのつまみ食いできるはず。
electronの裏動作にして、rtmp投げたり、h264 / aacに変換したりとかやってみたいし
webSocketサーバーにして同じく変換処理やらせたりやってみたいし・・・
MediaSource使わせるならfmp4とmkvのwriterつくっておきたいところですね。
まぁlibwebmとかlibmkvつかってもいいけど

とりあえずやっていこう・・・

bindingはrequireをかきやすくするだけだから、別にいらなかったかな。
まぁ、いいけど
nanはいる。
今回の肝

# 使い方
1. openh264とかfaacとかffmpegとか適当にインストールする。
2. https://github.com/taktod/ttLibC.git

これのdevelopのブランチを入手。

```
$ git clone https://github.com/taktod/ttLibC.git
$ cd ttLibC
$ git checkout develop
$ autoreconf
$ ./configure --enable-all --enable-gpl
$ make
$ make install
```

installはsudoいるかも？
configureの結果として、どういう動作が有効になったかわかるので、目的の変換ができるか確認するといいかも。

3. このプログラムがrebuildする。

```
$ git clone https://github.com/taktod/ttLibJsGyp.git
$ cd ttLibJsGyp
$ node-gyp rebuild
```

4. あとは適当にtestにあるjsを参考になにかする。

# 問題点

- vorbisのdecodeとかもやっておきたい。
- x265の出力とかもやりたい。
- rtmpクライアント動作つくっておきたい。

等々

# 確認済み

さらっとですが・・・
コンテナ
 flvの読み書き
 mkvの読み込み
 mp4の読み込み(普通のやつのみ、fmp4やMP4Box通したmp4はやってない。)
 mpegts読み書き(ただしマルチトラックは自信なし)
デコーダー()はすでに動作テスト済みのcodec
 audioConverter(aac)
 avcodecAudioDecoder(aac, mp3)
 avcodecVideoDecoder(vp8, flv1, vp6)
 openh264Decoder(h264)
 opusDecoder(opus)
 vtDecompressSessionDecoder(h264)
エンコーダー
 faacEncoder(aac)
 jpegEncoder(jpeg)
 mp3lameEncoder(mp3)
リサンプラ
 audioResampler(float -> shortのpcm変換)
 speexdspResampler(shortのpcmのサンプルレート変換)

# ライセンス
ttLibCのconfigureの結果に従う感じで
３条項BSDライセンスベースで
使うコーデックによってLGPLやGPLにする方向で
