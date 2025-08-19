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
#include "FractionalDelayLine.h"
#include "OnePole.hpp"

class CombFilter
{
public:
    void init(float maxDelayMs, double sampleRate);
    float processSample(float in);
    
    void setFrequency(float freq);
    void setFeedforwardDelay(float delayMs);
    void setFeedbackDelay(float delayMs);
    void setFeedforwardGain(float g) { gFf = g; }
    void setFeedbackGain(float g) { gFb = g; }
    void setDamping(float damping) { lpFilter.setCutoff(damping); };
    float getDamping() { return lpFilter.getCutoff(); };
    
    float getFeedbackGain() { return gFb; };
    float getFrequency() { return frequency; };

private:
    double sampleRate = 44100.0;
    float  gFf = 0.0f;
    float  gFb = 0.0f;
    float frequency;

    FractionalDelayLine ffDelay;
    FractionalDelayLine fbDelay;

    OnePole lpFilter;
    
    bool ready = false;
};

#endif /* CombFilter_h */
