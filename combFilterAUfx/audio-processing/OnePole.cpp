//
//  OnePole.cpp
//  templateAUfx
//
//  Created by Levin on 19.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#include "OnePole.hpp"
#include <cmath>

void OnePole::init(double cutoff, double sampleRate)
{
    this->sampleRate = sampleRate;
    setCutoff(cutoff);
}

void OnePole::setCutoff(float frequency)
{
    alpha = exp(-2.0 * M_PI * frequency / sampleRate);
    freq = frequency;
}

float OnePole::processSample(float sample)
{
    float out = (1.f - alpha) * sample + alpha * y_1;
    y_1 = out;
    return out;
}
