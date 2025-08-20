//
//  templateAUfxWithParametersAudioUnit.m
//  templateAUfxWithParameters
//
//  Created by mhamilt7 on 11/07/2018.
//  Copyright © 2018 mhamilt7. All rights reserved.
//
//==============================================================================
#import "combFilterAudioUnit.h"
#import <AVFoundation/AVFoundation.h>
#import "DSPKernel.hpp"
#import "BufferedAudioBus.hpp"
#import "ParameterAddresses.h"

//==============================================================================
@interface combFilterAudioUnit ()
@property (nonatomic, readwrite) AUParameterTree *parameterTree;
@property AUAudioUnitBus *outputBus;
@property AUAudioUnitBusArray *inputBusArray;
@property AUAudioUnitBusArray *outputBusArray ;
@end
//==============================================================================
@implementation combFilterAudioUnit
{
    // Add your C++ Classes Here:
    BufferedInputBus _inputBus;
    DSPKernel  _kernel;
}
@synthesize parameterTree = _parameterTree;

static inline AUValue shapeFeedback(AUValue v, AUValue k) {
    float a = fabsf(v);
    float shaped = 1.f - powf(1.f - a, k);
    return copysignf(shaped, v);
}

static inline AUValue logHz(AUValue linear, AUValue minHz, AUValue maxHz) {
    AUValue t = fmaxf(0.f, fminf(1.f, linear));
    AUValue lo = fmaxf(minHz, 1e-6f);
    AUValue hi = fmaxf(maxHz, lo * 1.0001f);
    AUValue logLo = logf(lo);
    AUValue logHi = logf(hi);
    return expf(logLo + (logHi - logLo) * t);
}

//==============================================================================
- (instancetype)initWithComponentDescription:(AudioComponentDescription)componentDescription options:(AudioComponentInstantiationOptions)options error:(NSError **)outError
{
    //--------------------------------------------------------------------------
    self = [super initWithComponentDescription:componentDescription
                                       options:options
                                         error:outError];
    if (self == nil) { return nil; }
    //--------------------------------------------------------------------------
    // @invalidname: Initialize a default format for the busses.
    AVAudioFormat *defaultFormat = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:44100.0
                                                                                  channels:2];
    //==========================================================================
    // Create parameter objects Here
    AUParameter *feedbackParam = [AUParameterTree createParameterWithIdentifier:@"feedback"
                                                                    name:@"Feedback"
                                                                        address:kParamFeedback
                                                                     min:-0.95
                                                                     max:0.95
                                                                    unit:kAudioUnitParameterUnit_Percent
                                                                unitName:nil
                                                                   flags:0
                                                            valueStrings:nil
                                                     dependentParameters:nil];
    feedbackParam.value = 0; // Initialize the parameter values.
    
    AUParameter *frequencyParam = [AUParameterTree createParameterWithIdentifier:@"frequency"
                                                                    name:@"Frequency"
                                                                         address:kParamFrequency
                                                                     min:0
                                                                     max:1
                                                                            unit:kAudioUnitParameterUnit_Hertz
                                                                unitName:nil
                                                                   flags:0
                                                            valueStrings:nil
                                                     dependentParameters:nil];
    feedbackParam.value = 0; // Initialize the parameter values.
    
    AUParameter *dampingParam = [AUParameterTree createParameterWithIdentifier:@"damping"
                                                                    name:@"Damping"
                                                                       address:kParamDamping
                                                                     min:0
                                                                     max:1
                                                                            unit:kAudioUnitParameterUnit_Hertz
                                                                unitName:nil
                                                                   flags:0
                                                            valueStrings:nil
                                                     dependentParameters:nil];
    
    dampingParam.value = 0;
    //--------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------
    _parameterTree = [AUParameterTree createTreeWithChildren:@[ feedbackParam, frequencyParam, dampingParam ]];  // Create the parameter tree.
    //--------------------------------------------------------------------------
    // Set .implementorStringFromValueCallback of a type
    // AUImplementorStringFromValueCallbackto to a pointer to a function
    // that represents the parameter value as a string
    _parameterTree.implementorStringFromValueCallback = ^(AUParameter *param, const AUValue *__nullable valuePtr)
    {
        AUValue value = valuePtr == nil ? param.value : *valuePtr;
        switch (param.address)
        {
            case kParamFeedback:
                return [NSString stringWithFormat:@"%.f", value];
            case kParamFrequency:
                return [NSString stringWithFormat:@"%.f", value];
            case kParamDamping:
                return [NSString stringWithFormat:@"%.f", value];
            default:
                return @"?";
        }
    };
    //--------------------------------------------------------------------------
     __block DSPKernel *localCaptureKernel = &_kernel; // Make local ptr of kernel to avoid capturing self.
    //--------------------------------------------------------------------------
    // implementorValueObserver is called when a parameter changes value.
    _parameterTree.implementorValueObserver = ^(AUParameter *param, AUValue value)
    {
        switch (param.address) {
            case kParamFeedback: {
                const AUValue k = 3.0f;
                AUValue coeff = shapeFeedback(value, k);
                localCaptureKernel->setParameter(param.address, coeff);
                break;
            }
            case kParamFrequency: {
                AUValue coeff = logHz(value, 20, 20000);
                localCaptureKernel->setParameter(param.address, coeff);
                break;
            }
            case kParamDamping: {
                AUValue coeff = logHz(value, 20, 20000);
                localCaptureKernel->setParameter(param.address, coeff);
                break;
            }
            default:
                localCaptureKernel->setParameter(param.address, value);
                break;
        }
    };
    //--------------------------------------------------------------------------
    // implementorValueProvider is called when the value needs to be refreshed.
    _parameterTree.implementorValueProvider = ^(AUParameter *param)
    {
        return localCaptureKernel->getParameter(param.address);
    };
    //==========================================================================
    _kernel.init(defaultFormat.channelCount, defaultFormat.sampleRate);
    //==========================================================================
    // Create the input and output busses.
    _inputBus.init(defaultFormat, 8);
    _outputBus = [[AUAudioUnitBus alloc] initWithFormat:defaultFormat error:nil];
    //--------------------------------------------------------------------------
    // Create the input and output bus arrays
    _inputBusArray  = [[AUAudioUnitBusArray alloc] initWithAudioUnit:self busType:AUAudioUnitBusTypeInput busses: @[_inputBus.bus]];
    _outputBusArray = [[AUAudioUnitBusArray alloc] initWithAudioUnit:self busType:AUAudioUnitBusTypeOutput busses: @[_outputBus]];
    //--------------------------------------------------------------------------
    self.maximumFramesToRender = 512;
    //--------------------------------------------------------------------------
    return self;
}

-(void)dealloc
{
    
}

//==============================================================================
#pragma mark - AUAudioUnit Overrides

- (AUAudioUnitBusArray *)inputBusses
{
    return _inputBusArray;
}

- (AUAudioUnitBusArray *)outputBusses
{
    return _outputBusArray;
}

- (BOOL)allocateRenderResourcesAndReturnError:(NSError **)outError
{
    //--------------------------------------------------------------------------
    if (![super allocateRenderResourcesAndReturnError:outError])
    {
        return NO;
    }
    //--------------------------------------------------------------------------
    if (self.outputBus.format.channelCount != _inputBus.bus.format.channelCount)
    {
        if (outError)
        {
            *outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:kAudioUnitErr_FailedInitialization userInfo:nil];
        }
        // Notify superclass that initialization was not successful
        self.renderResourcesAllocated = NO;
        
        return NO;
    }
    //--------------------------------------------------------------------------
    _inputBus.allocateRenderResources(self.maximumFramesToRender);
    //--------------------------------------------------------------------------
    _kernel.init(self.outputBus.format.channelCount, self.outputBus.format.sampleRate);
    //--------------------------------------------------------------------------
    
    return YES;
}
//==============================================================================
// Deallocate resources allocated in allocateRenderResourcesAndReturnError:
// Subclassers should call the superclass implementation.
- (void)deallocateRenderResources
{
    //--------------------------------------------------------------------------
    _inputBus.deallocateRenderResources();
    //--------------------------------------------------------------------------
    [super deallocateRenderResources];
}

//==============================================================================
#pragma mark - AUAudioUnit (AUAudioUnitImplementation)

// Block which subclassers must provide to implement rendering.
- (AUInternalRenderBlock)internalRenderBlock
{
    //--------------------------------------------------------------------------
    // C++ pointers: Referred to as 'captures', make them mutable with __block
    // Capture in locals to avoid ObjC member lookups.
    __block DSPKernel *state = &_kernel;
    __block BufferedInputBus *input = &_inputBus;
    //--------------------------------------------------------------------------
    return ^AUAudioUnitStatus (AudioUnitRenderActionFlags *actionFlags,
                               const AudioTimeStamp *timestamp,
                               AVAudioFrameCount frameCount,
                               NSInteger outputBusNumber,
                               AudioBufferList *outputData,
                               const AURenderEvent *realtimeEventListHead,
                               AURenderPullInputBlock pullInputBlock)
    {
        //----------------------------------------------------------------------
        AudioUnitRenderActionFlags pullFlags = 0;
        AUAudioUnitStatus err = input->pullInput(&pullFlags, timestamp, frameCount, 0, pullInputBlock);
        if (err != 0) { return err; }
        //----------------------------------------------------------------------
        AudioBufferList *inAudioBufferList = input->mutableAudioBufferList;
        AudioBufferList *outAudioBufferList = outputData;
        if (outputData->mBuffers[0].mData == nullptr)
        {
            for (UInt32 i = 0; i < outputData->mNumberBuffers; ++i)
            {
                outputData->mBuffers[i].mData = inAudioBufferList->mBuffers[i].mData;
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // DSP Goes in process method of DSPKernel
        state->processWithEvents(timestamp,
                                 frameCount,
                                 realtimeEventListHead,
                                 inAudioBufferList,
                                 outAudioBufferList);
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        return noErr;
    };
}
//==============================================================================
@end

