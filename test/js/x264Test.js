var ttg = require("../../");
var encoder = new ttg.encoder.X264(320, 240, "veryfast", "zerolatency", "main",
  {
    "qp-max": "50", "qp-min":4
  });
