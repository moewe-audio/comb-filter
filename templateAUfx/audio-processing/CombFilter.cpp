//
//  CombFilter.cpp
//  templateAUfx
//
//  Created by Levin on 13.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#include "CombFilter.hpp"
#include <algorithm>
#include <cmath>

void CombFilter::init(float maxDelayMs, double sr)
{
    sampleRate = sr;

    ffDelay.init(maxDelayMs, sampleRate);
    fbDelay.init(maxDelayMs, sampleRate);
    ready = true;
}

void CombFilter::setFeedforwardDelay(float delayMs)
{
    ffDelay.setDelayMs(delayMs);
}

void CombFilter::setFeedbackDelay(float delayMs)
{
    fbDelay.setDelayMs(delayMs);
}

float CombFilter::processSample(float in)
{
    if (!ready) return in;
    ffDelay.push(in);
    const float delayedX = ffDelay.read();
    const float delayedY = fbDelay.read();
    float out = in + gFf * delayedX - gFb * delayedY;
    fbDelay.push(out);
    return out;
}
