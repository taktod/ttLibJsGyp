var fs = require("fs");
var exec = require("child_process").execSync;
// 動作ターゲット defs:定義 libs:ライブラリ includes:ヘッダファイル
var target = process.argv[2];
// 設定ファイルを読み込んでどのようなコンパイルにするかコントロールする。
/*
{
  "target": "bsd" | "lgpl" | "gpl", // 動作モード指定
  "disable": ["jpeg", "faac" ...],  // 除外するライブラリ指定
  "searchPath": ["c:¥¥hogehoge¥¥"], // 検索に追加するパス
  "debug": true // debug出力するかどうか
}
 */
var setting = loadSetting();

/*
チェックすべきライブラリ
file socketについては、常に有効(emscriptenとかの別のコンパイルはしないので、考慮しない。)
opencv openalは放置
jpeg       any                jpeglib.h
openh264   any                pkgconfig
x264       gpl                pkgconfig
x265       gpl                pkgconfig
theora     any                pkgconfig
daala      any                pkgconfig
mp3lame    lgpl(enc) gpl(dec) lame/lame.h libmp3lame.a
faac       lgpl               faac.h
speexdsp   any                pkgconfig
speex      any                pkgconfig
opus       any                pkgconfig
faad       gpl                無視
vorbis     any                pkgconfig
avcodec    lgpl               pkgconfig
swscale    lgpl               pkgconfig
swresample lgpl               pkgconfig
apple      darwin only        darwin or not
webrtc     any?               無視(electronで攻めるので、必要ないはず)
soundtouch lgpl               pkgconfig
fdkaac     lgpl               
libyuv     any                libyuv.h libyuv.a
libpng     any                pkgconfig
こんなところか・・・
 */

checkDebug();
setupAvcodec();
setupApple();
setupFaac();
setupFdkaac();
setupJpeg(); 
setupMp3lame();
setupOpenh264();
setupOpus();
setupTheora();
setupSoundtouch();
setupSpeex();
setupSpeexdsp();
setupSwresample();
setupSwscale();
setupVorbis();
setupX264();
setupX265();
//setupDaala();
//setupFaad();
setupWin32();
setupLibYuv();
setupLibPng();

function checkDebug() {
  switch(target) {
  case "defs":
    if(setting["debug"]) {
      console.log("__DEBUG_FLAG__=1");
    }
    break;
  default:
    break;
  }
}
function setupAvcodec() {
  if(setting["targetValue"] < 1) {
    // lgpl以上じゃないと動作させない
    return;
  }
  if(setting["disable"].indexOf("avcodec") != -1) {
    // avcodecは無効になってる。
    return;
  }
  // とりあえず書くだけ書いてみよう
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists libavcodec libavutil && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_AVCODEC__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs libavcodec libavutil").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I libavcodec libavutil | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupApple() {
  if(setting["disable"].indexOf("apple") != -1) {
    return;
  }
  if(setting["os"] == "darwin") {
    // osxの場合はappleのframeworkを利用可能にしておく。
    switch(target) {
    case "defs":
      console.log("__ENABLE_APPLE__");
      break;
    case "libs":
      console.log("'-framework AudioToolbox' '-framework AudioUnit' '-framework VideoToolbox' '-framework CoreMedia' '-framework CoreFoundation' '-framework CoreVideo' '-framework CoreAudio'");
      break;
    case "includes":
      break;
    }
  }
}

function setupFaac() {
  if(setting["disable"].indexOf("faac") != -1) {
    return;
  }
  if(setting["targetValue"] == 0) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(checkFile("faac.h")
    && checkFile("faaccfg.h")
    && checkFile("libfaac.a")
    && (checkFile("libfaac.dylib") || checkFile("libfaac.so"))) {
      switch(target) {
      case "defs":
        console.log("__ENABLE_FAAC_ENCODE__");
        break;
      case "libs":
        setting["searchPath"].forEach(function(path) {
          console.log("-L" + path);
        });
        console.log("-lfaac");
        break;
      case "includes":
        setting["searchPath"].forEach(function(path) {
          console.log(path);
        });
        break;
      default:
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupFdkaac() {
  if(setting["disable"].indexOf("fdkaac") != -1) {
    // fdkaacが無効になってる
    return;
  }
  if(setting["targetValue"] == 0) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists fdk-aac && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_FDKAAC_ENCODE__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs fdk-aac").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I fdk-aac | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupJpeg() {
  if(setting["disable"].indexOf("jpeg") != -1) {
    // mp3が無効
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(checkFile("jpeglib.h")
    && checkFile("libjpeg.a")
    && (checkFile("libjpeg.so") || checkFile("libjpeg.dylib"))) {
      // jpegが使える
      switch(target) {
      case "defs":
        console.log("__ENABLE_JPEG__");
        break;
      case "libs":
        setting["searchPath"].forEach(function(path) {
          console.log("-L" + path);
        });
        console.log("-ljpeg");
        break;
      case "includes":
        setting["searchPath"].forEach(function(path) {
          console.log(path);
        });
        break;
      default:
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupMp3lame() {
  if(setting["disable"].indexOf("mp3lame") != -1) {
    // mp3が無効
    return;
  }
  if(setting["targetValue"] == 0) {
    // lgpl or gplではないので、スキップ
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(checkFile("lame/lame.h")
    && checkFile("libmp3lame.a")
    && (checkFile("libmp3lame.so") || checkFile("libmp3lame.dylib"))) {
      // mp3lameが使える
      switch(target) {
      case "defs":
        if(setting["targetValue"] == 2) {
          console.log("__ENABLE_MP3LAME_DECODE__");
        }
        console.log("__ENABLE_MP3LAME_ENCODE__");
        break;
      case "libs":
        setting["searchPath"].forEach(function(path) {
          console.log("-L" + path);
        });
        console.log("-lmp3lame");
        break;
      case "includes":
        setting["searchPath"].forEach(function(path) {
          console.log(path);
        });
        break;
      default:
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupOpenh264() {
  if(setting["disable"].indexOf("openh264") != -1) {
    // openh264は無効になってる
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists openh264 && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_OPENH264__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs openh264").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I openh264 | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupOpus() {
  if(setting["disable"].indexOf("opus") != -1) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists opus && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_OPUS__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs opus").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I opus | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupTheora() {
  if(setting["disable"].indexOf("theora") != -1) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists theora ogg && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_THEORA__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs theora ogg").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I theora ogg | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupSoundtouch() {
  if(setting["disable"].indexOf("soundtouch") != -1) {
    return;
  }
  if(setting["targetValue"] == 0) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists soundtouch && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_SOUNDTOUCH__ ST_NO_EXCEPTION_HANDLING");
        break;
      case "libs":
        console.log(exec("pkg-config --libs soundtouch").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I soundtouch | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupSpeex() {
  // これ・・・speexをoffにしてspeexdspをonにするいい方法ないね。
  if(setting["disable"].indexOf("speex") != -1) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists speex && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_SPEEX__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs speex").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I speex | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupSpeexdsp() {
  if(setting["disable"].indexOf("speexdsp") != -1) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists speexdsp && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_SPEEXDSP__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs speexdsp").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I speexdsp | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupSwresample() {
  if(setting["disable"].indexOf("swresample") != -1) {
    return;
  }
  if(setting["targetValue"] == 0) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists libswresample && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_SWRESAMPLE__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs libswresample").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I libswresample | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupSwscale() {
  if(setting["disable"].indexOf("swscale") != -1) {
    return;
  }
  if(setting["targetValue"] == 0) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists libswscale && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_SWSCALE__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs libswscale").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I libswscale | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupVorbis() {
  if(setting["disable"].indexOf("vorbis") != -1) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists vorbisenc vorbis && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_VORBIS_ENCODE__ __ENABLE_VORBIS_DECODE__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs vorbisenc vorbis").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I vorbisenc vorbis | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupX264() {
  if(setting["disable"].indexOf("x264") != -1) {
    return;
  }
  if(setting["targetValue"] != 2) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists x264 && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_X264__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs x264").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I x264 | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupX265() {
  if(setting["disable"].indexOf("x265") != -1) {
    return;
  }
  if(setting["targetValue"] != 2) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists x265 && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_X265__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs x265").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I x265 | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupLibYuv() {
  if(setting["disable"].indexOf("libyuv") != -1) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(checkFile("libyuv.h")
    && checkFile("libyuv.a")
    && (checkFile("libyuv.so") || checkFile("libyuv.dylib"))) {
      // libyuvが使える
      switch(target) {
      case "defs":
        console.log("__ENABLE_LIBYUV__");
        break;
      case "libs":
        setting["searchPath"].forEach(function(path) {
          console.log("-L" + path);
        });
        console.log("-lyuv");
        break;
      case "includes":
        setting["searchPath"].forEach(function(path) {
          console.log(path);
        });
        break;
      default:
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupLibPng() {
  if(setting["disable"].indexOf("libpng") != -1) {
    // libpngは無効になってる
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists libpng && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_LIBPNG__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs libpng").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I libpng | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  case "windows_nt":
  default:
    break;
  }
}

function setupWin32() {
  if(setting["disable"].indexOf("win32") != -1) {
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    break;
  case "windows":
  case "windows_nt":
    switch(target) {
    case "defs":
      console.log("__ENABLE_WIN32__");
      break;
    case "libs":
      break;
    case "includes":
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

function loadSetting() {
  var os = require("os");
  var setting = (function() {
    try {
      return JSON.parse(fs.readFileSync('../../takSetting.json'));
    }
    catch(e) {
      try {
        return JSON.parse(fs.readFileSync('takSetting.json'));
      }
      catch(e) {
        return {};
      }
    }
  })();
  if(!setting["target"]) {
    setting["target"] = "bsd";
  }
  switch(setting["target"]) {
  case "gpl":
    setting["targetValue"] = 2;
    break;
  case "lgpl":
    setting["targetValue"] = 1;
    break;
  default:
    setting["targetValue"] = 0;
    break;
  }
  if(!setting["disable"]) {
    setting["disable"] = [];
  }
  if(!setting["searchPath"]) {
    setting["searchPath"] = [];
  }
  setting["os"] = os.type().toString().toLowerCase();
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    setting["searchPath"].push("/usr/local/include");
    setting["searchPath"].push("/usr/local/lib");
    break;
  case "windows":
  case "windows_nt":
    break;
  default:
    break;
  }
  return setting;
}

function checkFile(file) {
  // setting["searchPath"]/fileが存在しているか確認し、ある場合はtrue、ない場合はfalseを返す
  var find = false;
  setting["searchPath"].forEach(function(path) {
    try {
      fs.statSync(path + "/" + file);
      find = true;
    }
    catch(e) {
      if(e.code === "ENOENT") {
        // 見つからなかった場合
        return;
      }
      throw e;
    }
  });
  return find;
}






/*
switch(os.type().toString().toLowerCase()) {
case "darwin":
  checkDarwin(target);
  break;
case "linux":
  checkLinux(target);
  break;
case "windows":
case "windows_nt":
  checkWindow(target);
  break;
default:
  return;
}

function checkDarwin(type) {
  // 必要なライブラリをリストアップしておく
  exec("");
}

function checkLinux(type) {

}

function checkWindow(type) {

}

*/