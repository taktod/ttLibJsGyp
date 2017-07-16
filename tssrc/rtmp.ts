import {Socket} from "net";
import {EventEmitter} from "events";
var Bootstrap = require("bindings")("ttLibJsGyp")["RtmpBootstrap"];

declare class RtmpBootstrap {
  public constructor(address:string, app:string);
  public connect(socket:Socket):void;
  public data(socket:Socket, data:Buffer):void;

  public createStream(func:{(streamId:number):void}):void;
  public play(streamId:number, name:string, hasVideo:boolean, hasAudio:boolean):void;
  public publish(streamId:number, name:string):void;
  public setBufferLength(streamId:number, length:number):void;
  public queueFrame(streamId:number, frame:any):void;
  public closeStream(streamId:number):void;
}

export class NetConnection extends EventEmitter {
  private socket:Socket;
  private bootstrap:RtmpBootstrap;
  constructor() {
    super();
    this.socket = null;
  }
  public connect(address:string):void {
    // このタイミングでsocketをつくってなにかする感じかね。
    this.socket = new Socket();
    this.socket["netStreams"] = {};
    this.socket["tettyWrite"] = (buffer:Buffer) => {
      this.socket.write(buffer);
    };
    this.socket["tettyEventCallback"] = (streamId:number, event:any) => {
      if(streamId == -1) {
        this.emit("onStatusEvent", event);
      }
      else {
        this.socket["netStreams"][streamId].emit("onStatusEvent", event);
      }
    };
    this.socket["tettyFrameCallback"] = (streamId:number, frame:any) => {
      if(streamId == -1) {
        return;
      }
      else {
        this.socket["netStream"][streamId].emit("onFrameCallback", frame);
      }
    };
    this.socket.setKeepAlive(true);
    this.socket.setNoDelay(true);
    this.socket.on("connect", () => {
      // このタイミングでbootstrap_connectを実施
      this.bootstrap.connect(this.socket);
    });
    this.socket.on("data", (data:Buffer) => {
      // このタイミングでbootstrap_updateを実施
      this.bootstrap.data(this.socket, data);
    });
    this.socket.on("close", (hasError:boolean) => {
      // 明示的になにかすることはないとしようと思う。
      this.bootstrap = null; // これでデストラクタが動作すると思う。
      // まぁここにくることはないだろうけど
    });
    this.socket.on("error", (error) => {
      // エラーが発生したときには、Connect.Failedを送ってやることにする。
      this.emit("onStatusEvent", {info:{code:"NetConnection.Connect.Failed"}});
    });
    // このタイミングでbootstrapをつくって、紐づけておきます。
    // new Bootstrap(); // bootstrap_makeを実施 bootstrap_channelを実施
    var res = address.match(/rtmp.?:\/\/([^/:]+)(:([0-9]+))?\/(.*)/);
    var host = res[1];
    var port = 1935;
    var app = res[4];
    if(res[3] != "" && res[3] != undefined) {
      port = parseInt(res[3]);
    }
    // ここで渡さない方がいいかも・・・
    this.bootstrap = new Bootstrap(address, app);
    // pipelineの構築は裏で実施する。
    // addressそのものとaddressからappを切り出すのの２つが必要になる。
    this.socket.connect(port, host);
  }
  public refSocket():Socket {
    return this.socket;
  }
  public refBootstrap():RtmpBootstrap {
    return this.bootstrap;
  }
}

export class NetStream extends EventEmitter {
  // scopeの問題でちゃんと保存されると思うけど。
  private socket:Socket;
  private bootstrap:RtmpBootstrap;
  private streamId:number;
  private orderCache:Array<{order:Function, params:Array<any>}>;
  constructor(nc:NetConnection) {
    super();
    this.socket = nc.refSocket();
    this.socket.on("error", () => {
      this.socket = null;
    });
    this.streamId = -1;
    this.bootstrap = nc.refBootstrap();
    this.orderCache = [];
    // ここでnetStreamをつくる必要がある。
    // promise使おうかと思ったけど、コンストラクタなので、無理か・・・
    this.bootstrap.createStream((streamId:number):void => {
      this.streamId = streamId;
      this.socket["netStreams"][streamId] = this;
      if(this.orderCache.length != 0) {
        // orderを実行する。
        this.orderCache.forEach((order) => {
          order.params.unshift(streamId);
          // orderはすべてbootstrapの配下なので、こうしとく。
          order.order.apply(this.bootstrap, order.params);
        });
      }
    });
  }
  public play(
      name:string,
      video:boolean=true,
      audio:boolean=true):void {
    // streamIdとの紐づけが重要。あとはどうでもいい感じか・・・
    if(this.streamId == -1) {
      this.orderCache.push({order:this.bootstrap.play, params:[name, video, audio]});
    }
    else {
      if(this.socket == null) {
        return;
      }
      this.bootstrap.play(this.streamId, name, video, audio);
    }
  }
  public publish(name:string):void {
    if(this.streamId == -1) {
      this.orderCache.push({order:this.bootstrap.publish, params:[name]});
    }
    else {
      if(this.socket == null) {
        return;
      }
      this.bootstrap.publish(this.streamId, name);
    }
  }
  public setBufferLength(length:number):void {
    if(this.streamId == -1) {
      this.orderCache.push({order:this.bootstrap.setBufferLength, params:[length]});
    }
    else {
      if(this.socket == null) {
        return;
      }
      this.bootstrap.setBufferLength(this.streamId, length);
    }
  }
  public queueFrame(jsFrame:any) {
    if(this.streamId == -1) {
      this.orderCache.push({order:this.bootstrap.queueFrame, params:[jsFrame]});
    }
    else {
      if(this.socket == null) {
        return;
      }
      this.bootstrap.queueFrame(this.streamId, jsFrame);
    }
  }
  public close():void {
    if(this.streamId == -1) {
      this.orderCache.push({order:this.bootstrap.closeStream, params:[]});
    }
    else {
      if(this.socket == null) {
        return;
      }
      this.bootstrap.closeStream(this.streamId);
    }
  }
}
