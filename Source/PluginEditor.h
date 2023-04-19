/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DelayControls.h"

//==============================================================================
/**
*/
class HandyFXAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::ComboBox::Listener, public juce::Timer
{
public:
    HandyFXAudioProcessorEditor (HandyFXAudioProcessor&);
    ~HandyFXAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HandyFXAudioProcessor& audioProcessor;

    juce::Label currentBPMLabel;

    juce::ComboBox effectSelector;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged);
    std::unique_ptr<DelayControls> delayControls;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HandyFXAudioProcessorEditor)
};
