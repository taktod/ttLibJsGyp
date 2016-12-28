{
    "variables": {
        'audioConverterDecoder':      "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/audioConverterDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'avcodecDecoder':             "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/avcodecDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'jpegDecoder':                "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/jpegDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'mp3lameDecoder':             "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/mp3lameDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'openh264Decoder':            "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/openh264Decoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'opusDecoder':                "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/opusDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'speexDecoder':               "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/speexDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'theoraDecoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/theoraDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'vorbisDecoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/vorbisDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'vtDecompressSessionDecoder': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/decoder/vtDecompressSessionDecoder.h 1>/dev/null 2>&1 && echo yes || echo no)",

        'audioConverterEncoder':    "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/audioConverterEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'avcodecEncoder':           "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/avcodecEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'faacEncoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/faacEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'jpegEncoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/jpegEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'mp3lameEncoder':           "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/mp3lameEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'openh264Encoder':          "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/openh264Encoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'opusEncoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/opusEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'speexEncoder':             "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/speexEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'theoraEncoder':            "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/theoraEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'vorbisEncoder':            "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/vorbisEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'vtCompressSessionEncoder': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/vtCompressSessionEncoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'x264Encoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/x264Encoder.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'x265Encoder':              "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/encoder/x265Encoder.h 1>/dev/null 2>&1 && echo yes || echo no)",

        'rtmpClient': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g'`/ttLibC/net/client/rtmp.h 1>/dev/null 2>&1 && echo yes || echo no)",

        'speexdspResampler': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/resampler/speexdspResampler.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'imageResizer':      "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/resampler/imageResizer.h 1>/dev/null 2>&1 && echo yes || echo no)",

        'opencvUtil':    "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/util/opencvUtil.h 1>/dev/null 2>&1 && echo yes || echo no)",
        'audioUnitUtil': "<!(ls `pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g' | sed -e 's/ //g'`/ttLibC/util/audioUnitUtil.h 1>/dev/null 2>&1 && echo yes || echo no)"
    },
    "targets": [
        {
            "target_name": 'valueDefine',
            "sources": [
                "src/c/value.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"
            ],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)',
            ]
        },
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
            "target_name": 'mkvWriter',
            "sources": [
                "src/c/container/mkvWriter.cpp",
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
            "target_name": 'webmWriter',
            "sources": [
                "src/c/container/webmWriter.cpp",
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
                    "defines": ["__ENABLE__"],
                    'include_dirs': [
                        "<!(pkg-config libavcodec --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs libavcodec)']
                }
            ]],
            "target_name": 'avcodecAudioDecoder',
            "sources": [
                "src/c/decoder/avcodecAudioDecoder.cpp",
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
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config libavcodec --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs libavcodec)']
                }
            ]],
            "target_name": 'avcodecVideoDecoder',
            "sources": [
                "src/c/decoder/avcodecVideoDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'openh264Decoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config openh264 --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs openh264)']
                }
            ]],
            "target_name": 'openh264Decoder',
            "sources": [
                "src/c/decoder/openh264Decoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'opusDecoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config opus --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs opus)']
                }
            ]],
            "target_name": 'opusDecoder',
            "sources": [
                "src/c/decoder/opusDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'theoraDecoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config theora --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs theora)']
                }
            ]],
            "target_name": 'theoraDecoder',
            "sources": [
                "src/c/decoder/theoraDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'vorbisDecoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config vorbis --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs vorbis)']
                }
            ]],
            "target_name": 'vorbisDecoder',
            "sources": [
                "src/c/decoder/vorbisDecoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
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
                    "defines": ["__ENABLE__"],
                    'libraries': [
                        '-lfaac']
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
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'jpegEncoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    'libraries': [
                        '-ljpeg']
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
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'mp3lameEncoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    'libraries': [
                        '-lmp3lame']
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
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'openh264Encoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config openh264 --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs openh264)']
                }
            ]],
            "target_name": 'openh264Encoder',
            "sources": [
                "src/c/encoder/openh264Encoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'opusEncoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config opus --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs opus)']
                }
            ]],
            "target_name": 'opusEncoder',
            "sources": [
                "src/c/encoder/opusEncoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'theoraEncoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config theora --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs theora)']
                }
            ]],
            "target_name": 'theoraEncoder',
            "sources": [
                "src/c/encoder/theoraEncoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'x264Encoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config x264 --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs x264)']
                }
            ]],
            "target_name": 'x264Encoder',
            "sources": [
                "src/c/encoder/x264Encoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'x265Encoder=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config x265 --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs x265)']
                }
            ]],
            "target_name": 'x265Encoder',
            "sources": [
                "src/c/encoder/x265Encoder.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
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
                'imageResizer=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'bgrImageResizer',
            "sources": [
                "src/c/resampler/bgrImageResizer.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "target_name": 'imageResampler',
            "sources": [
                "src/c/resampler/imageResampler.cpp",
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
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config speexdsp --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs speexdsp)']
                }
            ]],
            "target_name": 'speexdspResampler',
            "sources": [
                "src/c/resampler/speexdspResampler.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'imageResizer=="yes"', {
                    "defines": ["__ENABLE__"]
                }
            ]],
            "target_name": 'yuvImageResizer',
            "sources": [
                "src/c/resampler/yuvImageResizer.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },

        {
            "conditions": [[
                'audioUnitUtil=="yes"', {
                    "defines": ["__ENABLE__"],
                    'libraries': [
                        '-framework AudioToolbox -framework AudioUnit -framework CoreMedia -framework CoreFoundation -framework CoreAudio']
                }
            ]],
            "target_name": 'audioUnitPlayer',
            "sources": [
                "src/c/util/audioUnitPlayer.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'opencvUtil=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config opencv --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs opencv)']
                }
            ]],
            "target_name": 'opencvCapture',
            "sources": [
                "src/c/util/opencvCapture.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        },
        {
            "conditions": [[
                'opencvUtil=="yes"', {
                    "defines": ["__ENABLE__"],
                    "include_dirs": [
                        "<!(pkg-config opencv --cflags-only-I | sed -e 's/\-I//g')"],
                    'libraries': [
                        '<!@(pkg-config --libs opencv)']
                }
            ]],
            "target_name": 'opencvWindow',
            "sources": [
                "src/c/util/opencvWindow.cpp",
                "src/c/frame/frame.cpp"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "<!(pkg-config ttLibC --cflags-only-I | sed -e 's/\-I//g')"],
            'libraries': [
                '<!@(pkg-config --libs ttLibC)']
        }
    ]
}
