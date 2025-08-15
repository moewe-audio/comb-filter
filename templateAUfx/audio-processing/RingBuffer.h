//
//  RingBuffer.hpp
//  templateAUfx
//
//  Created by Levin on 13.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#ifndef RingBuffer_hpp
#define RingBuffer_hpp

#include <stdio.h>
#include <vector>

using namespace std;

class RingBuffer {
public:
    RingBuffer() = default;

    void init(size_t maxCapacity) {
        bitMask = maxCapacity - 1;
        data.assign(maxCapacity, 0.0f);
    }

    void reset() noexcept { head = 0; }

    inline void pushSample(float sample) {
        data[head & bitMask] = sample;
        ++head;
    }

    inline float getDelayedSample(size_t delay) const
    {
        size_t idx = (head - 1 - delay) & bitMask;
        return data[idx];
    }

private:
    vector<float> data;
    size_t cap  = 0;
    size_t bitMask = 0;
    size_t head = 0;
};

#endif /* RingBuffer_hpp */
