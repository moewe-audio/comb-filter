//
//  OnePole.hpp
//  templateAUfx
//
//  Created by Levin on 19.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#ifndef OnePole_hpp
#define OnePole_hpp

#include <stdio.h>

class OnePole {
public:
    OnePole() = default;
    
    void init(double cutoff, double sampleRate);
    float processSample(float sample);
    
    void setCutoff(float frequency);
    float getCutoff() { return freq; };
    
private:
    float y_1 = 0.f;
    float alpha = 0.f;
    float sampleRate;
    float freq = 0.f;
};

#endif /* OnePole_hpp */
