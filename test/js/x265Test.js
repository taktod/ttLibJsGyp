var ttg = require("../../");
var encoder = new ttg.encoder.X265(320, 240, "ultrafast", "zerolatency", "main",
  {
    "qpmax": "50", "qpmin":4
  });
