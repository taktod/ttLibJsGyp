# ttLibJsGyp

# 作者

taktod

mail poepoemix@hotmail.com
twitter https://twitter.com/taktod

# 内容

nodeでc言語を利用するためのライブラリ
メディアデータを読み込んだり、フレームのエンコード・デコードを実施したりできます。

ttLibJsGyp2のデータをこちら側にもってきました。

もともとのttLibJsGypの動作では

- ttLibCのプログラムは別でインストールする必要があった
- frameのデータもjsのBufferとして扱っていた
- それぞれのencoderの動作が独立して共有archiveになっていた

となっていました。

今回更新したプログラムでは

- ttLibCのプログラムをsubmoduleとして取り込んだ
- frameのデータはc言語のポインタとして取り回すことでbinaryコピーを回避
- すべての動作で1つのarchiveとした。

としました。

かつwindowsでも利用可能になり、windowsのMediaFoundationによるencodeの実装や
WasAPIによるaudioLoopback機能等盛り込んでいます。

# 使い方

nodeのnpmのプロジェクトを適当につくる。
node-gypでコンパイル可能にしておく

```
$ npm install taktod/ttLibJsGyp
```
で必要なプログラムがインストール & コンパイルされ利用可能になります。

# ライセンス

利用するライブラリに従ってライセンスが変化します。
GPLv3を利用する場合、すべての機能が利用できます。
LGPLv3を利用する場合は、一部の機能x264等が利用できなくなります。
3-Clause BSDを利用する場合は、さらにffmpegのコードやfdkaac faac等が利用できなくなります。

利用するライブラリはtakSetting.jsonというファイルを置くことで決定できます。

詳しくはlibcheck.jsをみてください

