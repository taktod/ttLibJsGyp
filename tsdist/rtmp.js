"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
exports.__esModule = true;
var net_1 = require("net");
var events_1 = require("events");
var Bootstrap = require("bindings")("ttLibJsGyp")["RtmpBootstrap"];
var NetConnection = (function (_super) {
    __extends(NetConnection, _super);
    function NetConnection() {
        var _this = _super.call(this) || this;
        _this.socket = null;
        return _this;
    }
    NetConnection.prototype.connect = function (address) {
        var _this = this;
        // このタイミングでsocketをつくってなにかする感じかね。
        this.socket = new net_1.Socket();
        this.socket["netStreams"] = {};
        this.socket["tettyWrite"] = function (buffer) {
            _this.socket.write(buffer);
        };
        this.socket["tettyEventCallback"] = function (streamId, event) {
            if (streamId == -1) {
                _this.emit("onStatusEvent", event);
            }
            else {
                _this.socket["netStreams"][streamId].emit("onStatusEvent", event);
            }
        };
        this.socket["tettyFrameCallback"] = function (streamId, frame) {
            if (streamId == -1) {
                return;
            }
            else {
                _this.socket["netStream"][streamId].emit("onFrameCallback", frame);
            }
        };
        this.socket.setKeepAlive(true);
        this.socket.setNoDelay(true);
        this.socket.on("connect", function () {
            // このタイミングでbootstrap_connectを実施
            _this.bootstrap.connect(_this.socket);
        });
        this.socket.on("data", function (data) {
            // このタイミングでbootstrap_updateを実施
            _this.bootstrap.data(_this.socket, data);
        });
        this.socket.on("close", function (hasError) {
            // 明示的になにかすることはないとしようと思う。
            _this.bootstrap = null; // これでデストラクタが動作すると思う。
            // まぁここにくることはないだろうけど
        });
        this.socket.on("error", function (error) {
            // エラーが発生したときには、Connect.Failedを送ってやることにする。
            _this.emit("onStatusEvent", { info: { code: "NetConnection.Connect.Failed" } });
        });
        // このタイミングでbootstrapをつくって、紐づけておきます。
        // new Bootstrap(); // bootstrap_makeを実施 bootstrap_channelを実施
        var res = address.match(/rtmp.?:\/\/([^/:]+)(:([0-9]+))?\/(.*)/);
        var host = res[1];
        var port = 1935;
        var app = res[4];
        if (res[3] != "" && res[3] != undefined) {
            port = parseInt(res[3]);
        }
        // ここで渡さない方がいいかも・・・
        this.bootstrap = new Bootstrap(address, app);
        // pipelineの構築は裏で実施する。
        // addressそのものとaddressからappを切り出すのの２つが必要になる。
        this.socket.connect(port, host);
    };
    NetConnection.prototype.refSocket = function () {
        return this.socket;
    };
    NetConnection.prototype.refBootstrap = function () {
        return this.bootstrap;
    };
    return NetConnection;
}(events_1.EventEmitter));
exports.NetConnection = NetConnection;
var NetStream = (function (_super) {
    __extends(NetStream, _super);
    function NetStream(nc) {
        var _this = _super.call(this) || this;
        _this.socket = nc.refSocket();
        _this.socket.on("error", function () {
            _this.socket = null;
        });
        _this.streamId = -1;
        _this.bootstrap = nc.refBootstrap();
        _this.orderCache = [];
        // ここでnetStreamをつくる必要がある。
        // promise使おうかと思ったけど、コンストラクタなので、無理か・・・
        _this.bootstrap.createStream(function (streamId) {
            _this.streamId = streamId;
            _this.socket["netStreams"][streamId] = _this;
            if (_this.orderCache.length != 0) {
                // orderを実行する。
                _this.orderCache.forEach(function (order) {
                    order.params.unshift(streamId);
                    // orderはすべてbootstrapの配下なので、こうしとく。
                    order.order.apply(_this.bootstrap, order.params);
                });
            }
        });
        return _this;
    }
    NetStream.prototype.play = function (name, video, audio) {
        if (video === void 0) { video = true; }
        if (audio === void 0) { audio = true; }
        // streamIdとの紐づけが重要。あとはどうでもいい感じか・・・
        if (this.streamId == -1) {
            this.orderCache.push({ order: this.bootstrap.play, params: [name, video, audio] });
        }
        else {
            if (this.socket == null) {
                return;
            }
            this.bootstrap.play(this.streamId, name, video, audio);
        }
    };
    NetStream.prototype.publish = function (name) {
        if (this.streamId == -1) {
            this.orderCache.push({ order: this.bootstrap.publish, params: [name] });
        }
        else {
            if (this.socket == null) {
                return;
            }
            this.bootstrap.publish(this.streamId, name);
        }
    };
    NetStream.prototype.setBufferLength = function (length) {
        if (this.streamId == -1) {
            this.orderCache.push({ order: this.bootstrap.setBufferLength, params: [length] });
        }
        else {
            if (this.socket == null) {
                return;
            }
            this.bootstrap.setBufferLength(this.streamId, length);
        }
    };
    NetStream.prototype.queueFrame = function (jsFrame) {
        if (this.streamId == -1) {
            this.orderCache.push({ order: this.bootstrap.queueFrame, params: [jsFrame] });
        }
        else {
            if (this.socket == null) {
                return;
            }
            this.bootstrap.queueFrame(this.streamId, jsFrame);
        }
    };
    NetStream.prototype.close = function () {
        if (this.streamId == -1) {
            this.orderCache.push({ order: this.bootstrap.closeStream, params: [] });
        }
        else {
            if (this.socket == null) {
                return;
            }
            this.bootstrap.closeStream(this.streamId);
        }
    };
    return NetStream;
}(events_1.EventEmitter));
exports.NetStream = NetStream;
