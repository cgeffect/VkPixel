//
//  MogicTTSImpl.hpp
//  vulkan
//
//  Created by Jason on 2022/3/31.
//

#ifndef MogicTTSImpl_hpp
#define MogicTTSImpl_hpp

#include <stdio.h>
#include "IMogicTTSImpl.h"

namespace mogic {

class MogicTTSImpl:public IMogicTTSImpl {
private:
    /* data */
public:
    MogicTTSImpl(/* args */);
    ~MogicTTSImpl();
    
    int ttsMake(const std::string &text) override;
};

} // namespace mogic
#endif /* MogicTTSImpl_hpp */
