//
//  DSPKernel.hpp
//  AudioUnitV3Template
//
//  Created by mhamilt7 on 10/07/2018.
//  Copyright © 2018 mhamilt7. All rights reserved.
//
//==============================================================================
#ifndef DSPKernel_h
#define DSPKernel_h
//==============================================================================
#include "CombFilter.hpp"
#include <vector>
#include "combFilterAudioUnit.h"
#include "ParameterAddresses.h"
//==============================================================================
/*
 DSPKernel Performs our filter signal processing.
 As a non-ObjC class, this is safe to use from render thread.
 */
class DSPKernel
{
public:
    //==========================================================================
    // MARK: Member Functions
    DSPKernel() {}
    
    //==========================================================================
    void init(int channelCount, double inSampleRate)
    {
        channels = channelCount;
        sampleRate = float(inSampleRate);
        stereoComb.clear();
        stereoComb.reserve(channelCount);
        for (int i = 0; i < channelCount; ++i) {
            stereoComb.emplace_back();
            stereoComb[i].init(40, sampleRate);
            stereoComb[i].setFrequency(440);
            stereoComb[i].setFeedforwardGain(0.f);
            stereoComb[i].setFeedbackGain(0.f);
        }
    }
    //==============================================================================
    /**
     The central DSP Algorithm: this is essentially 'where the magic happens'
     
     @param frameCount from the DSPKernel super class. This is the framesRemaining
     which is the 'AVAudioFrameCount frameCount' directly from
     internalRenderBlock in the main FilterDemo.mm file.
     
     @param bufferOffset  bufferOffset = frameCount - framesRemaining. frameCount
     from internalRenderBlock is copied and subtracted from
     processWithEvents in the DSPKernel in which the call to
     this function is made
     */
    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset)
    {
        int channelCount = channels;
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) // For each sample.
        {
            int frameOffset = int(frameIndex + bufferOffset);
            for (int channel = 0; channel < channelCount; ++channel)
            {
                float* in  = (float*)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
                float* out = (float*)outBufferListPtr->mBuffers[channel].mData + frameOffset;
                const float x = *in;
                const float xDelayed = stereoComb[channel].processSample(x);
                *out = xDelayed;
            }
        }
    }
    //==============================================================================
    void processWithEvents(AudioTimeStamp const *timestamp,
                           AUAudioFrameCount frameCount,
                           AURenderEvent const *events,
                           AudioBufferList* inBufferList,
                           AudioBufferList* outBufferList)
    {
        //----------------------------------------------------------------------
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
        //----------------------------------------------------------------------
        AUEventSampleTime now = AUEventSampleTime(timestamp->mSampleTime);
        AUAudioFrameCount framesRemaining = frameCount;
        AURenderEvent const *event = events;
        //----------------------------------------------------------------------
        while (framesRemaining > 0)
        {
            // If there are no more events, we can process the entire remaining segment and exit.
            if (event == nullptr)
            {
                AUAudioFrameCount const bufferOffset = frameCount - framesRemaining;
                process(framesRemaining, bufferOffset);
                return;
            }
            
            // **** start late events late.
            auto timeZero = AUEventSampleTime(0);
            auto headEventTime = event->head.eventSampleTime;
            AUAudioFrameCount const framesThisSegment = AUAudioFrameCount(std::max(timeZero, headEventTime - now));
            
            // Compute everything before the next event.
            if (framesThisSegment > 0)
            {
                AUAudioFrameCount const bufferOffset = frameCount - framesRemaining;
                process(framesThisSegment, bufferOffset);
                framesRemaining -= framesThisSegment;    // Advance frames.
                now += AUEventSampleTime(framesThisSegment); // Advance time.
            }
            // Not sample-accurate, but fine for my little demo...
            while (event) {
                if (event->head.eventType == AURenderEventMIDI) {
                    
                    AUMIDIEvent midiEvent = event->MIDI;
                    uint8_t message = midiEvent.data[0] & 0xF0;
                    uint8_t data1 = midiEvent.data[1];
                    uint8_t data2 = midiEvent.data[2];
                    
                    if (message == 0x90 && data2 > 0) { // Note On
                        float hz = 440.0f * powf(2.0f, (int(data1) - 69) / 12.0f);
                        setParameter(kParamFrequency, AUValue(hz));
                    }
                    event = event->head.next;
                }
            }
        }
    }
    //==========================================================================
    // Getters and Setters
    /**
     Set Parameter Value which is a member variable of the DSPKernel object,
     given its address (an in that can be set with an enum)
     @param address addree of parameter of type AUParameterAddress
     @param value value to set parameter to
     */
    void setParameter(AUParameterAddress address, AUValue value)
    {
        if (address == kParamFeedback)
        {
            for (CombFilter &comb : stereoComb)
            {
                comb.setFeedbackGain(value);
            }
        } else if (address == kParamFrequency)
        {
            for (CombFilter &comb : stereoComb)
            {
                comb.setFrequency(value);
            }
        } else if (address == kParamDamping)
        {
            for (CombFilter &comb : stereoComb)
            {
                comb.setDamping(value);
            }
        }
    }
    
    /**
     Get the parameter value given its address
     
     @param address an unsigned int
     @return AUValue (i.e. a float)
     */
    AUValue getParameter(AUParameterAddress address)
    {
//        OSAtomicIncremesnt32Barrier(&changeCounter); // add in
        if (address == kParamFeedback)
        {
            return stereoComb[0].getFeedbackGain();
        } else if (address == kParamFrequency)
        {
            return stereoComb[0].getFrequency();
        } else if (address == kParamFrequency)
        {
            return stereoComb[0].getDamping();
        }
        return 0;
    }
    
private:
    //==========================================================================
    // MARK: Member Variables
    float sampleRate = 44100.0;
    int channels;
    AudioBufferList* inBufferListPtr = nullptr;
    AudioBufferList* outBufferListPtr = nullptr;
    std::vector<CombFilter> stereoComb;
public:
    //==========================================================================
};

#endif /* DSPKernel_h */
