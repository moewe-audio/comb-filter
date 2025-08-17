//
//  AudioUnitViewController.swift
//  templateAUfxWithParameters
//
//  Created by mhamilt7 on 11/07/2018.
//  Copyright © 2018 mhamilt7. All rights reserved.
//
//==============================================================================
import CoreAudioKit
//==============================================================================
public class AudioUnitViewController: AUViewController, AUAudioUnitFactory
{
    //==========================================================================
    @IBOutlet var feedbackSlider: NSSlider!
    @IBOutlet var frequencySlider: NSSlider!
    //==========================================================================
    var audioUnit: AUAudioUnit?
    //==========================================================================
    public override func viewDidLoad()
    {
        super.viewDidLoad()
        view.wantsLayer = true
        view.layer?.backgroundColor = NSColor.darkGray.cgColor
        NSLog("Finished viewDidLoad()", NSNull())
        if audioUnit == nil
        {
            return
        }
    }
    
    public override func awakeFromNib()
    {
            NSLog("Awake From NIB", NSNull())
    }
    
    //==========================================================================
    public func createAudioUnit(with componentDescription: AudioComponentDescription) throws -> AUAudioUnit
    {
        NSLog("Create Audio Unit", NSNull())
        audioUnit = try templateAUfxWithParametersAudioUnit(componentDescription: componentDescription, options: [])
        return audioUnit!
    }
    //==========================================================================

    @IBAction func handleFeedbackSliderValueChanged(_ sender: NSSlider)
    {
        guard let modulatorUnit = audioUnit as? templateAUfxWithParametersAudioUnit else {return}
        guard let feedbackParam = modulatorUnit.parameterTree?.parameter(withAddress: myFeedbackParam) else { return }
        feedbackParam.setValue(sender.floatValue, originator: nil)
    }
    
    @IBAction func handleFrequencySliderValueChanged(_ sender: NSSlider)
    {
        guard let modulatorUnit = audioUnit as? templateAUfxWithParametersAudioUnit else {return}
        guard let freqParam = modulatorUnit.parameterTree?.parameter(withAddress: myFrequencyParam) else { return }
        freqParam.setValue(sender.floatValue, originator: nil)
    }
    //==========================================================================
}
