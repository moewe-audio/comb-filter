//
//  RingBuffer.hpp
//  templateAUfx
//
//  Created by Levin on 13.08.25.
//  Copyright © 2025 mhamilt7. All rights reserved.
//

#ifndef RingBuffer_hpp
#define RingBuffer_hpp

#include <vector>
#include <cstddef>

class RingBuffer {
public:
    RingBuffer() = default;

    void init(std::size_t capacity) {
        cap = capacity > 0 ? capacity : 1;
        data.assign(cap, 0.0f);
        head = 0;
    }

    void reset() noexcept {
        head = 0;
        std::fill(data.begin(), data.end(), 0.0f);
    }

    void pushSample(float sample) {
        data[head % cap] = sample;
        ++head;
    }

    float getDelayedSample(std::size_t delay) const {
        // head points to the *next* write position
        // last written sample is at head-1
        std::size_t pos = (head == 0 ? 0 : (head - 1));
        std::size_t idx = (pos + cap - (delay % cap)) % cap;
        return data[idx];
    }

    std::size_t capacity() const noexcept { return cap; }

private:
    std::vector<float> data;
    std::size_t cap  = 0;
    std::size_t head = 0;
};

#endif /* RingBuffer_hpp */
