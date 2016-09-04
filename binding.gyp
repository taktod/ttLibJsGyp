{
    "variables": {
        'audioConverterDecoder':      "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/audioConverterDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'avcodecDecoder':             "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/avcodecDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'jpegDecoder':                "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/jpegDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'mp3lameDecoder':             "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/mp3lameDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'openh264Decoder':            "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/openh264Decoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'opusDecoder':                "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/opusDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'speexDecoder':               "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/speexDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'theoraDecoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/theoraDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'vorbisDecoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/vorbisDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'vtDecompressSessionDecoder': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/decoder/vtDecompressSessionDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",

        'audioConverterEncoder':    "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/audioConverterEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'avcodecEncoder':           "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/avcodecEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'faacEncoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/faacEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'jpegEncoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/jpegEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'mp3lameEncoder':           "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/mp3lameEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'openh264Encoder':          "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/openh264Encoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'opusEncoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/opusEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'speexEncoder':             "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/speexEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'theoraEncoder':            "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/theoraEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'vorbisEncoder':            "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/vorbisEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'vtCompressSessionEncoder': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/vtCompressSessionEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'x264Encoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/x264Encoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'x265Encoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/encoder/x265Encoder.h 1>/dev/null 2>&1 && echo yes || echo no)",

        'rtmpClient': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/net/client/rtmp.h 1>/dev/null 2>&1 && echo yes || echo no)",

        'speexdspResampler': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/resampler/speexdspResampler.h 1>/dev/null 2>&1 && echo yes || echo no)"
    },
    "targets": [
        {
            "target_name": 'flvReader',
            "sources": [
                "src/c/container/flvReader.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },
        {
            "target_name": 'flvWriter',
            "sources": [
                "src/c/container/flvWriter.cpp",
                "src/c/frame/frame.cpp",
                "src/c/util/binary.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },
        {
            "target_name": 'mkvReader',
            "sources": [
                "src/c/container/mkvReader.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },
        {
            "target_name": 'mp4Reader',
            "sources": [
                "src/c/container/mp4Reader.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },
        {
            "target_name": 'mp4Writer',
            "sources": [
                "src/c/container/mp4Writer.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },
        {
            "target_name": 'mpegtsReader',
            "sources": [
                "src/c/container/mpegtsReader.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },
        {
            "target_name": 'mpegtsWriter',
            "sources": [
                "src/c/container/mpegtsWriter.cpp",
                "src/c/frame/frame.cpp",
                "src/c/util/binary.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },

        {
            "conditions": [[
                'audioConverterDecoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    'libraries': [
                        '-framework AudioToolbox -framework AudioUnit -framework CoreMedia -framework CoreFoundation -framework CoreAudio']
                }
            ]],
            "target_name": 'audioConverterDecoder',
            "sources": [
                "src/c/decoder/audioConverterDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'avcodecDecoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'avcodecAudioDecoder',
            "sources": [
                "src/c/decoder/avcodecAudioDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config libavcodec --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs libavcodec)']
        },
        {
            "conditions": [[
                'avcodecDecoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'avcodecVideoDecoder',
            "sources": [
                "src/c/decoder/avcodecVideoDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config libavcodec --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs libavcodec)']
        },
        {
            "conditions": [[
                'openh264Decoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'openh264Decoder',
            "sources": [
                "src/c/decoder/openh264Decoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config openh264 --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs openh264)']
        },
        {
            "conditions": [[
                'opusDecoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'opusDecoder',
            "sources": [
                "src/c/decoder/opusDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config opus --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs opus)']
        },
        {
            "conditions": [[
                'theoraDecoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'theoraDecoder',
            "sources": [
                "src/c/decoder/theoraDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config theora --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs theora)']
        },
        {
            "conditions": [[
                'vorbisDecoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'vorbisDecoder',
            "sources": [
                "src/c/decoder/vorbisDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config vorbis --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs vorbis)']
        },
        {
            "conditions": [[
                'vtDecompressSessionDecoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    'libraries': [
                        '-framework VideoToolbox -framework CoreMedia -framework CoreFoundation -framework CoreVideo']
                }
            ]],
            "target_name": 'vtDecompressSessionDecoder',
            "sources": [
                "src/c/decoder/vtDecompressSessionDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },

        {
            "conditions": [[
                'faacEncoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'faacEncoder',
            "sources": [
                "src/c/encoder/faacEncoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '-lfaac']
        },
        {
            "conditions": [[
                'jpegEncoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'jpegEncoder',
            "sources": [
                "src/c/encoder/jpegEncoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '-ljpeg']
        },
        {
            "conditions": [[
                'mp3lameEncoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'mp3lameEncoder',
            "sources": [
                "src/c/encoder/mp3lameEncoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '-lmp3lame']
        },
        {
            "conditions": [[
                'openh264Encoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'openh264Encoder',
            "sources": [
                "src/c/encoder/openh264Encoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config openh264 --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs openh264)']
        },
        {
            "conditions": [[
                'opusEncoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'opusEncoder',
            "sources": [
                "src/c/encoder/opusEncoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config opus --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs opus)']
        },
        {
            "conditions": [[
                'theoraEncoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'theoraEncoder',
            "sources": [
                "src/c/encoder/theoraEncoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config theora --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs theora)']
        },
        {
            "conditions": [[
                'x264Encoder=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'x264Encoder',
            "sources": [
                "src/c/encoder/x264Encoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config x264 --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs x264)']
        },
        {
            "conditions": [[
                'rtmpClient=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'rtmpClient',
            "sources": [
                "src/c/net/rtmpClient.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "target_name": 'audioResampler',
            "sources": [
                "src/c/resampler/audioResampler.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },
        {
            "conditions": [[
                'speexdspResampler=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'speexdspResampler',
            "sources": [
                "src/c/resampler/speexdspResampler.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')",
                "<!(pkg-config speexdsp --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
                '<!@(pkg-config --libs speexdsp)']
        }
    ]
}
