#pragma once

#include <JuceHeader.h>
#include "D:/Programming/VS/HandyFX/Source/PluginProcessor.h"

//==============================================================================

/**
*/

class VibratoControls : public juce::Component,
	public juce::Slider::Listener,
	public juce::ComboBox::Listener,
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
	void comboBoxChanged(juce::ComboBox* comboBox) override;
	void buttonClicked(juce::Button* button) override;

private:
	HandyFXAudioProcessor& audioProcessor;
	std::unique_ptr<juce::Slider> deepnessSlider;
	std::unique_ptr<juce::ComboBox> vibrationComboBox;
	std::unique_ptr<juce::Label> deepnessLabel;
	std::unique_ptr<juce::Label> vibrationLabel;
};
