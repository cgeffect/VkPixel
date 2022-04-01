//
//  MogicTTS.hpp
//  mogic
//
//  Created by Jason on 2022/3/31.
//

#ifndef MogicTTS_hpp
#define MogicTTS_hpp

#include <stdio.h>
#include <string>
#include "IMogicTTSImpl.h"

namespace mogic {

enum MogicTTSFormat {
    MOGIC_TTS_FMT_PCM = 0, // defalt
    MOGIC_TTS_FMT_MP3 = 1,
    MOGIC_TTS_FMT_WAV = 2,
};

//合成音频采样率
enum MogicTTSSampleRate {
    MOGIC_TTS_SAMPLE_RATE_8K = 0,
    MOGIC_TTS_SAMPLE_RATE_16K = 1, // defalt
};

//合成音频数字发音
enum MogicTTSRDN {
    MOGIC_TTS_RDN_ZERO = 0, // defalt 数值优先
    MOGIC_TTS_RDN_ONE = 1, //完全数值
    MOGIC_TTS_RDN_TWO = 2, //完全字符串
    MOGIC_TTS_RDN_THREE = 3, //字符串优先
};

class MogicTTS {
private:
    IMogicTTSImpl *impl;
    /* data */
public:
    MogicTTS(/* args */);
    ~MogicTTS();

    int ttsParamConfig(int voiceName, int speed, int volume, int rdn, int backgroundSound, MogicTTSSampleRate rate);

    int ttsMake(const std::string &text);
};

} // namespace mogic
#endif /* MogicTTS_hpp */
