/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


#pragma comment(lib, "aubio.lib")

//==============================================================================
HandyFXAudioProcessor::HandyFXAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : MagicProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )

#endif
{
    parameters.state = juce::ValueTree(juce::Identifier("HandyFXPlugin"));
    //addParameter(feedbackParam = new juce::AudioParameterFloat("Feedback", "Feedback", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 0.7f));
    //addParameter(delayParam = new juce::AudioParameterFloat("Delay", "Delay", juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f), 1.f));
    //addParameter(tempoSyncParam = new juce::AudioParameterBool("TempoSync", "Tempo Sync", false));
    //addParameter(delayDivParam = new juce::AudioParameterChoice("DelayDiv", "Delay Div", juce::StringArray("1/1", "3/4", "1/2", "1/4", "1/8", "1/16"), 0));
}

HandyFXAudioProcessor::~HandyFXAudioProcessor()
{
}

//==============================================================================
const juce::String HandyFXAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HandyFXAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HandyFXAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HandyFXAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HandyFXAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HandyFXAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HandyFXAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HandyFXAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HandyFXAudioProcessor::getProgramName (int index)
{
    return {};
}

void HandyFXAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HandyFXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto delayBufferSize = static_cast<int>(sampleRate * 2.0);
    delayBuffer.setSize(getTotalNumOutputChannels(), delayBufferSize);
    magicState.prepareToPlay(sampleRate, samplesPerBlock);
    aubioWrapper.initialiseWrapper(sampleRate, samplesPerBlock);
}

void HandyFXAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HandyFXAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HandyFXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    aubioWrapper.detectBeats(buffer);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {

        fillCircularBuffer(buffer, channel);
        readCircularBuffer(buffer, delayBuffer, channel);
        fillCircularBuffer(buffer, channel);

    }

    updateWritePosition(buffer, delayBuffer);

}

void HandyFXAudioProcessor::readCircularBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel)
{
    float feedback = *parameters.getRawParameterValue("Feedback");
    bool tempoSync = /*parameters.getRawParameterValue("TempoSync")->load();*/true;
    float delayTime;
    if (tempoSync == true) {
        delayTime = aubioWrapper.getDelayTime(getDelayTimeFraction(*parameters.getRawParameterValue("DelayDiv")));
    }
    else {
        delayTime = *parameters.getRawParameterValue("Delay");
    }
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    auto readPosition = writePosition - delayTime * getSampleRate();
    float g = feedback;
    if (readPosition < 0)
        readPosition += delayBufferSize;

    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, g, g);
    }
    else
    {

        auto numToEnd = delayBufferSize - readPosition;
        auto numFromStart = bufferSize - numToEnd;
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), numToEnd, g, g);
        buffer.addFromWithRamp(channel, numToEnd, delayBuffer.getReadPointer(channel, 0), numFromStart, g, g);

    }

}

void HandyFXAudioProcessor::fillCircularBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
    auto* channelData = buffer.getWritePointer(channel);
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    if (writePosition + bufferSize < delayBufferSize)
    {
        delayBuffer.copyFromWithRamp(channel, writePosition, channelData, bufferSize, 0.1f, 0.1f);
    }

    else {
        auto spaceLeft = delayBufferSize - writePosition;
        auto samplesFromStart = bufferSize - spaceLeft;
        delayBuffer.copyFromWithRamp(channel, writePosition, channelData, spaceLeft, 0.1f, 0.1f);
        delayBuffer.copyFromWithRamp(channel, 0, channelData, samplesFromStart, 0.1f, 0.1f);
    }

}
//==============================================================================

void HandyFXAudioProcessor::updateWritePosition(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout HandyFXAudioProcessor::createParameterLayout() 
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<juce::AudioParameterFloat>("Delay", "Delay", 0.0f, 2.0f, 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("Feedback", "Feedback", 0.0f, 1.0f, 0.5f));
	layout.add(std::make_unique<juce::AudioParameterBool>("TempoSync", "Tempo Sync", false));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("DelayDiv"), "Delay Div", juce::StringArray{ "1/8", "1/4", "1/2", "3/4", "1/1", "2/1" }, 0));
	return layout;
}

float HandyFXAudioProcessor::getDelayTimeFraction(int index)
{
    switch (index)
    {
    case 0: return 1.0f / 8.0f;
    case 1: return 1.0f / 4.0f;
    case 2: return 1.0f / 2.0f;
    case 3: return 3.0f / 4.0f;
    case 4: return 1.0f;
    case 5: return 2.0f;
    default: return 1.0f;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HandyFXAudioProcessor();
}
