#pragma once

#include <JuceHeader.h>
#include "D:/Programming/VS/HandyFX/Source/PluginProcessor.h"

//==============================================================================

/**
*/

class VibratoControls : public juce::Component,
	public juce::Slider::Listener,
	public juce::Button::Listener
{
public:
	VibratoControls(HandyFXAudioProcessor& p);
	~VibratoControls() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;
	//==============================================================================
	void sliderValueChanged(juce::Slider* slider) override;
	void buttonClicked(juce::Button* button) override;

private:
	HandyFXAudioProcessor& audioProcessor;
	std::unique_ptr<juce::Slider> rateSlider;
	std::unique_ptr<juce::Slider> depthSlider;
	std::unique_ptr<juce::Slider> delaySlider;
	std::unique_ptr<juce::Slider> feedbackSlider;
	std::unique_ptr<juce::Slider> mixSlider;
	std::unique_ptr<juce::ToggleButton> vibratoButton;
	std::unique_ptr<juce::ToggleButton> chorusButton;
};
