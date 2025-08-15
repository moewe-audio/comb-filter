//
//  CombFilter.cpp
//  templateAUfx
//
//  Created by Levin on 13.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#include "CombFilter.hpp"
#include <cmath>
#include <algorithm>

void CombFilter::init(float maxDelayMs, double sampleRate)
{
    this->sampleRate = sampleRate;
    maxDelaySamples = std::max(0.0, maxDelayMs * sampleRate / 1000.0);
    delayBuffer.init(maxDelaySamples + 2);
    ready = true;
}

void CombFilter::setDelay(float delayMs)
{
    double d = std::max(0.0, delayMs * sampleRate / 1000.0);
    double dMaxForLinear = static_cast<double>(maxDelaySamples);
    if (d > dMaxForLinear) d = dMaxForLinear;

    delaySamples = static_cast<int>(std::floor(d));
    fractionalDelaySamples = static_cast<float>(d - delaySamples);

    if (delaySamples + 1 > maxDelaySamples) {
        delaySamples = maxDelaySamples;
        fractionalDelaySamples = 0.0f;
    }
}

float CombFilter::processSample(float in)
{
    delayBuffer.pushSample(in);
    float delayed = readFrac();
    return in + gain * delayed;
}

float CombFilter::readFrac()
{
    const float s0 = delayBuffer.getDelayedSample(delaySamples);
    const float s1 = delayBuffer.getDelayedSample(delaySamples + 1);
    // MARK: Use more elaborate interpolation
    return (1.0f - fractionalDelaySamples) * s0 + ( fractionalDelaySamples) * s1;
}
