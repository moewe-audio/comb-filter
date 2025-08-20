# AUv3 Demo: CombFilter

A little audio unit v3 demo plugin inspired by Elektron's CombFilter on the Octatrack.
[Quick Demo]()

## Features
+ tuning based on frequency
+ frequency can be controlled via a Midi Keyboard
    + Plugin is a "musical effect" 
+ positive and negative polarity
+ damping parameter

## Usage
+ to use without MIDI control simply insert plugin as an audio effect on the desired audio track
+ to control frequency with MIDI insert plugin on an instrument track and route audio through it using the sidechain setting (Logic Pro X)

## Implementation notes
The plugin is based on the [Audio-Unit-V3-Templates](https://github.com/mhamilt/Audio-Unit-V3-Templates) to kick off development. Therefore, the core structure was provided by this template. 
My work mostly lies in:
+ MIDI parsing and audio processing: `combFilterAUfx/DSPKernel`
+ DSP: `combFilterAUfx/audio-processing/*`
+ additional parameters: `CombFilter/combFilterAudioUnit.m`
