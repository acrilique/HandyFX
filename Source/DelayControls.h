#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

/**
*/

class DelayControls : public juce::Component,
	public juce::Slider::Listener,
	public juce::ComboBox::Listener,
	public juce::Button::Listener
{
public:
	DelayControls(HandyFXAudioProcessor& p);
	~DelayControls() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;
	//==============================================================================
	void sliderValueChanged(juce::Slider* slider) override;
	void comboBoxChanged(juce::ComboBox* comboBox) override;
	void buttonClicked(juce::Button* button) override;

private:
	HandyFXAudioProcessor& audioProcessor;
	std::unique_ptr<juce::ToggleButton> tempoSyncButton;
	std::unique_ptr<juce::Slider> feedbackSlider;
	std::unique_ptr<juce::Slider> delaySlider;
	std::unique_ptr<juce::ComboBox> delayDivComboBox;
	std::unique_ptr<juce::Label> feedbackLabel;
	std::unique_ptr<juce::Label> delayLabel;
};