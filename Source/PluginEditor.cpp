/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HandyFXAudioProcessorEditor::HandyFXAudioProcessorEditor (HandyFXAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    currentBPMLabel.setText("Current BPM: " + std::to_string(audioProcessor.getBPM()), juce::dontSendNotification);
    addAndMakeVisible(currentBPMLabel);

    addAndMakeVisible(effectSelector);
    effectSelector.addItem("Delay", 1);
    effectSelector.addItem("Reverb", 2);
    effectSelector.addItem("Vibrato", 3);
    effectSelector.addListener(this);

    effectSelector.setSelectedId(1); // Set "Delay" as the default value

    delayControls = std::make_unique<DelayControls>(p);
    addAndMakeVisible(delayControls.get());

    reverbControls = std::make_unique<ReverbControls>(p);
    addAndMakeVisible(reverbControls.get());

    delayControls.get()->setVisible(true);
    reverbControls.get()->setVisible(false);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 600);

    startTimer(300);
}

HandyFXAudioProcessorEditor::~HandyFXAudioProcessorEditor()
{
}

void HandyFXAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &effectSelector)
    {
        switch (effectSelector.getSelectedId())
        {
            case 1:
			    delayControls.get()->setVisible(true);
			    reverbControls.get()->setVisible(false);
			break;
            case 2:
                delayControls.get()->setVisible(false);
                reverbControls.get()->setVisible(true);
            break;
            case 3:
				delayControls.get()->setVisible(false);
				reverbControls.get()->setVisible(false);
        }
	}
}

//==============================================================================
void HandyFXAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void HandyFXAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    effectSelector.setBounds(10, 10, getWidth() - 20, 30);
    delayControls->setBounds(10, 50, getWidth() - 20, getHeight() - 60);
    currentBPMLabel.setBounds(10, getHeight() - 40, getWidth() - 20, 30);

}

void HandyFXAudioProcessorEditor::timerCallback()
{
	currentBPMLabel.setText("Current BPM: " + std::to_string(audioProcessor.getBPM()), juce::dontSendNotification);
}