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
    
    float getFeedbackGain() { return gFb; };

private:
    double sampleRate = 44100.0;
    float  gFf = 0.0f;
    float  gFb = 0.0f;

    FractionalDelayLine ffDelay;
    FractionalDelayLine fbDelay;

    bool ready = false;
};

#endif /* CombFilter_h */
