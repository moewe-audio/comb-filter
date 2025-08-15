//
//  FractionalDelayLine.h
//  AudioUnitV3Template
//
//  Created by Levin on 15.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#ifndef FractionalDelayLine_h
#define FractionalDelayLine_h

#include "RingBuffer.h"

class FractionalDelayLine {
public:
    
    FractionalDelayLine() = default;
    
    void init(float maxDelayMs, double sampleRate)
    {
        this->sampleRate = sampleRate;
        const double maxSamplesF = std::max(0.0, static_cast<double>(maxDelayMs) * sampleRate / 1000.0);
        maxDelaySamples = static_cast<int>(std::floor(maxSamplesF));

        // headroom for Lagrange extra samples
        const int minNeeded = 3;
        if (maxDelaySamples < minNeeded)
            maxDelaySamples = minNeeded;

        // extra samples for simpler wrap around reads
        const int extra = 4;
        const int size  = maxDelaySamples + extra;

        buff.init(size);
        setDelayMs(0.0f);
        ready = true;
    }
    
    void setDelayMs(float delayMs)
    {
        if (!ready) return;

        const double dRaw = std::max(0.0, static_cast<double>(delayMs) * sampleRate / 1000.0);
        const double dMaxForLag = static_cast<double>(maxDelaySamples - 2);
        const double d = std::min(dRaw, std::max(0.0, dMaxForLag));
        delayInt = static_cast<int>(std::floor(d));
        delayFrac = static_cast<float>(d - delayInt);
        if (delayInt + 2 > maxDelaySamples)
        {
            delayInt = std::max(0, maxDelaySamples - 2);
            delayFrac = 0.0f;
        }
    }
    
    void push(float x)
    {
        if (!ready) return;
        
    }
    
    float read() const
    {
        if (!ready) return 0.0f;

        const float y0 = getDelayedSample(delayInt);
        const float y1 = getDelayedSample(delayInt + 1);
        const float y2 = getDelayedSample(delayInt + 2);

        const float x = delayFrac;
        // L0 = (x-1)(x-2)/2
        // L1 = -x(x-2)
        // L2 =  x(x-1)/2
        const float L0 = (x - 1.0f) * (x - 2.0f) * 0.5f;
        const float L1 = -x * (x - 2.0f);
        const float L2 =  x * (x - 1.0f) * 0.5f;
        return y0 * L0 + y1 * L1 + y2 * L2;
    }
    
private:
    
    float getDelayedSample(int n) const
    {
        return buff.getDelayedSample(n);
    }
    
    int maxDelaySamples;
    int delaySamples;
    float delayFrac;
    int delayInt;
    double sampleRate;
    RingBuffer buff;
    bool ready = false;
};

#endif /* FractionalDelayLine_h */
