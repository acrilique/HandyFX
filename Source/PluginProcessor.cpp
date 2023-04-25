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
     : AudioProcessor (BusesProperties()
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
    setReverbParams();
    setVibratoParams();
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
bool HandyFXAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HandyFXAudioProcessor::createEditor()
{
    return new HandyFXAudioProcessorEditor(*this);
}

//==============================================================================
void HandyFXAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

}

void HandyFXAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

}


//==============================================================================
void HandyFXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto delayBufferSize = static_cast<int>(sampleRate * 2.0);
    delayBuffer.setSize(getTotalNumOutputChannels(), delayBufferSize);
    
    aubioWrapper.initialiseWrapper(sampleRate, samplesPerBlock);

    reverb.setParameters(reverbParams);
    setVibratoParams();

    reverb.setEnabled(true);

    juce::dsp::ProcessSpec spec{};
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    reverb.prepare(spec);
    vibrato.prepare(spec);

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

    applyChosenEffect(buffer, totalNumInputChannels);

}

void HandyFXAudioProcessor::applyChosenEffect(juce::AudioBuffer<float>& buffer, int inputChannels)
{
    int effect = *parameters.getRawParameterValue("Effect");
    if (effect == 0) { // Delay
        for (int channel = 0; channel < inputChannels; ++channel)
        {
			fillCircularBuffer(buffer, channel);
			readCircularBuffer(buffer, delayBuffer, channel);
			fillCircularBuffer(buffer, channel);
		}
		updateWritePosition(buffer, delayBuffer);
	}
    else if (effect == 1 || effect == 2) { 
		
        juce::dsp::AudioBlock<float> block(buffer);
		juce::dsp::ProcessContextReplacing<float> context(block);
        
        if (effect == 1) { // Reverb
            setReverbParams();
            reverb.process(context);
        }
		else if (effect == 2) { // Vibrato
        	setVibratoParams();
        	vibrato.process(context);
        }
    }
}

void HandyFXAudioProcessor::readCircularBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel)
{

    feedbackSmoothed.setTargetValue(*parameters.getRawParameterValue("Feedback"));
    bool tempoSync = parameters.getRawParameterValue("TempoSync")->load();
    
    if (tempoSync == true) {
        delaySmoothed.setTargetValue(aubioWrapper.getDelayTime(getDelayTimeFraction(*parameters.getRawParameterValue("DelayDiv"))));
    }
    else {
        delaySmoothed.setTargetValue(*parameters.getRawParameterValue("Delay"));
    }

    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    const float delayTime = delaySmoothed.getNextValue();
    auto readPosition = writePosition - delayTime * getSampleRate();
    const float g = feedbackSmoothed.getNextValue();
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
void HandyFXAudioProcessor::setReverbParams()
{
	reverbParams.roomSize = *parameters.getRawParameterValue("RoomSize");
	reverbParams.damping = *parameters.getRawParameterValue("Damping");
	reverbParams.width = *parameters.getRawParameterValue("Width");
	reverbParams.dryLevel = *parameters.getRawParameterValue("DryLevel");
	reverbParams.wetLevel = *parameters.getRawParameterValue("WetLevel");
	reverb.setParameters(reverbParams);
}

void HandyFXAudioProcessor::setVibratoParams()
{
	vibrato.setRate(*parameters.getRawParameterValue("Rate"));
    vibrato.setDepth(*parameters.getRawParameterValue("Depth"));
    vibrato.setCentreDelay(*parameters.getRawParameterValue("CentreDelay"));
    vibrato.setFeedback(*parameters.getRawParameterValue("VibratoFeedback"));
    vibrato.setMix(*parameters.getRawParameterValue("Mix"));
}
//==============================================================================
// 

juce::AudioProcessorValueTreeState::ParameterLayout HandyFXAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;

    // Add the ComboBox parameter
    params.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("Effect"), "Effect", juce::StringArray{ "Delay", "Reverb", "Vibrato" }, 0));

    // Add the effect-specific groups

    params.add(createDelayParameterGroup());
    params.add(createReverbParameterGroup());
    params.add(createVibratoParameterGroup());

    return params;
}


// Delay Parameter Group
std::unique_ptr<juce::AudioProcessorParameterGroup> HandyFXAudioProcessor::createDelayParameterGroup()
{
    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("Delay", "Delay", "|");

    // Add your delay-specific parameters here
    // Example: group->addChild(std::make_unique<AudioParameterFloat>(...));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("Delay", "Delay", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("Feedback", "Feedback", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 0.7f));
    group->addChild(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("DelayDiv"), "Delay Div", juce::StringArray{ "1/8", "1/4", "1/2", "3/4", "1", "2" }, 0));
    group->addChild(std::make_unique<juce::AudioParameterBool>("TempoSync", "Tempo Sync", false));

    return group;
}

// Reverb Parameter Group
std::unique_ptr<juce::AudioProcessorParameterGroup> HandyFXAudioProcessor::createReverbParameterGroup()
{
    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("reverb", "Reverb", "|");

    // Add your reverb-specific parameters here
    group->addChild(std::make_unique<juce::AudioParameterFloat>("RoomSize", "Room Size", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("Damping", "Damping", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("Width", "Width", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("DryLevel", "Dry Level", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("WetLevel", "Wet Level", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    return group;
}

// Vibrato Parameter Group
std::unique_ptr<juce::AudioProcessorParameterGroup> HandyFXAudioProcessor::createVibratoParameterGroup()
{
    auto group = std::make_unique<juce::AudioProcessorParameterGroup>("vibrato", "Vibrato", "|");

    // Add your vibrato-specific parameters here
    group->addChild(std::make_unique<juce::AudioParameterFloat>("Rate", "Rate", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 5.0f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("Depth", "Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("CentreDelay", "Centre Delay", juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f), 8.0f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("VibratoFeedback", "Feedback", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    group->addChild(std::make_unique<juce::AudioParameterFloat>("Mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    return group;
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

float HandyFXAudioProcessor::getBPM()
{
    float detected_bpm = aubioWrapper.getBPM();
    if (detected_bpm > 160.0f)
        detected_bpm /= 2.0f;
    else if (detected_bpm < 80.0f)
        detected_bpm *= 2.0f;
	return detected_bpm;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HandyFXAudioProcessor();
}
