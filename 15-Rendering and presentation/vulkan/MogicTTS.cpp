//
//  MogicTTS.cpp
//  mogic
//
//  Created by Jason on 2022/3/31.
//

#include "MogicTTS.h"
//#include "src/video/MogicDefines.h"
#include "MogicTTSImpl.h"

namespace mogic {

MogicTTS::MogicTTS(/* args */) {
}

MogicTTS::~MogicTTS() {
}

int MogicTTS::ttsParamConfig(int voiceName, int speed, int volume, int rdn, int backgroundSound, MogicTTSSampleRate rate) {
    if (speed < 0 || speed > 100) {
        // MOGIC_CHE
    }
    if (volume < 0 || volume > 100) {
    }

    return 0;
}

int MogicTTS::ttsMake(const std::string &text) {
    
    int type = 0;
    if (type == 0) {
        this->impl = new MogicTTSImpl();
    } else {
        this->impl = new MogicTTSImpl();
    }
    
    impl->ttsMake(text);
    
    return 0;
}

} // namespace mogic
