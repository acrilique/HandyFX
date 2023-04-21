#pragma once

#include <JuceHeader.h>
#include "D:/Programming/VS/HandyFX/Source/PluginProcessor.h"

class ReverbControls : public juce::Component,
	public juce::Slider::Listener
{	
public:
	ReverbControls(HandyFXAudioProcessor& p);
	~ReverbControls();
	
	void paint(juce::Graphics&);
	void resized();

	void sliderValueChanged(juce::Slider* slider);

private:
	HandyFXAudioProcessor& audioProcessor;
	std::unique_ptr<juce::Slider> roomSizeSlider;
	std::unique_ptr<juce::Slider> dampingSlider;
	std::unique_ptr<juce::Slider> widthSlider;
	std::unique_ptr<juce::Slider> wetLevelSlider;
	std::unique_ptr<juce::Slider> dryLevelSlider;

};

