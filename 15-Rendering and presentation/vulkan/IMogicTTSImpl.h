//
//  IMogicTTSImpl.hpp
//  vulkan
//
//  Created by Jason on 2022/3/31.
//

#ifndef IMogicTTSImpl_hpp
#define IMogicTTSImpl_hpp

#include <stdio.h>
#include <string>

namespace mogic {
class IMogicTTSImpl {
    
public:
    virtual ~IMogicTTSImpl();
        
    virtual int ttsMake(const std::string &text) = 0;
};
}
#endif /* IMogicTTSImpl_hpp */
