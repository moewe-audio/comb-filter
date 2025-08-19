//
//  ParameterAddresses.h
//  AudioUnitV3Template
//
//  Created by Levin on 19.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#import <AudioToolbox/AUParameters.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef NS_ENUM(AUParameterAddress, ParameterAddress) {
    kParamFeedback  = 0,
    kParamFrequency = 1,
    kParamDamping = 2,
};

#ifdef __cplusplus
}
#endif
