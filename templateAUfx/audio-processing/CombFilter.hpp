//
//  combFilter.hpp
//  AudioUnitV3Template
//
//  Created by Levin on 12.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#ifndef CombFilter_h
#define CombFilter_h

#include <vector>
#include "RingBuffer.h"

class CombFilter {
public:
    CombFilter() = default;
    void init(float maxDelayMs, double sampleRate);
    void setDelay(float delayMs);
    float processSample(float in);
    
private:
    float readFrac();
    
    std::vector<float> delayedSamples;
    int delaySamples;
    int maxDelaySamples;
    float fractionalDelaySamples;
    float gain = 0.3;
    double sampleRate;
    bool ready = false;
    RingBuffer delayBuffer;
};

#endif /* CombFilter_h */
