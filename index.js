module.exports = {
    flv: {
        Reader: require("bindings")("flvReader").FlvReader,
        Writer: require("bindings")("flvWriter").FlvWriter
    },
    mkv: {
        Reader: require("bindings")("mkvReader").MkvReader
    },
    mp4: {
        Reader: require("bindings")("mp4Reader").Mp4Reader,
        Writer: require("bindings")("mp4Writer").Mp4Writer
    },
    mpegts: {
        Reader: require("bindings")("mpegtsReader").MpegtsReader,
        Writer: require("bindings")("mpegtsWriter").MpegtsWriter
    },
    decoder: {
        AudioConverter:      require("bindings")("audioConverterDecoder").AudioConverterDecoder,
        AvcodecAudio:        require("bindings")("avcodecAudioDecoder").AvcodecAudioDecoder,
        AvcodecVideo:        require("bindings")("avcodecVideoDecoder").AvcodecVideoDecoder,
        Openh264:            require("bindings")("openh264Decoder").Openh264Decoder,
        Opus:                require("bindings")("opusDecoder").OpusDecoder,
        Theora:              require("bindings")("theoraDecoder").TheoraDecoder,
        Vorbis:              require("bindings")("vorbisDecoder").VorbisDecoder,
        VtDecompressSession: require("bindings")("vtDecompressSessionDecoder").VtDecompressSessionDecoder
    },
    encoder: {
        Faac:     require("bindings")("faacEncoder").FaacEncoder,
        Jpeg:     require("bindings")("jpegEncoder").JpegEncoder,
        Mp3lame:  require("bindings")("mp3lameEncoder").Mp3lameEncoder,
        Openh264: require("bindings")("openh264Encoder").Openh264Encoder,
        Opus:     require("bindings")("opusEncoder").OpusEncoder,
        Theora:   require("bindings")("theoraEncoder").TheoraEncoder,
        X264:     require("bindings")("x264Encoder").X264Encoder
    },
    net: {
        RtmpClient: require("bindings")("rtmpClient")
    },
    resampler: {
        Audio: require("bindings")("audioResampler").AudioResampler,
        Speexdsp: require("bindings")("speexdspResampler").SpeexdspResampler
    }
};
