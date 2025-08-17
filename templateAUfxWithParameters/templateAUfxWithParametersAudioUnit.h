//
//  templateAUfxWithParametersAudioUnit.h
//  templateAUfxWithParameters
//
//  Created by mhamilt7 on 11/07/2018.
//  Copyright © 2018 mhamilt7. All rights reserved.
//
//==============================================================================
#import <AudioToolbox/AudioToolbox.h>
//==============================================================================
// Define parameter addresses.
extern const AUParameterAddress myFeedbackParam;
extern const AUParameterAddress myFrequencyParam;
//==============================================================================
@interface templateAUfxWithParametersAudioUnit : AUAudioUnit

@end
//==============================================================================
