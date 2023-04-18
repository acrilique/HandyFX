/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AubioWrapper.h"

//==============================================================================
/**
*/
class HandyFXAudioProcessor  : public foleys::MagicProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    HandyFXAudioProcessor();
    ~HandyFXAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void HandyFXAudioProcessor::readCircularBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel);
    void HandyFXAudioProcessor::fillCircularBuffer(juce::AudioBuffer<float>& buffer, int channel);
    void HandyFXAudioProcessor::updateWritePosition(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer);

    //==============================================================================
    //juce::AudioParameterFloat *delayParam = nullptr;
    //juce::AudioParameterFloat *feedbackParam = nullptr;
    //juce::AudioParameterBool *tempoSyncParam = nullptr;
    //juce::AudioParameterChoice *delayDivParam = nullptr;
    //juce::AudioProcessorParameter *wetDryParam = nullptr;

private:
    int writePosition = { 0 };
    juce::AudioBuffer<float> delayBuffer;
    AubioWrapper aubioWrapper;

    //==============================================================================
    float getDelayTimeFraction(int index);

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState parameters{ *this, nullptr, "Parameters", createParameterLayout() };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HandyFXAudioProcessor)
};
