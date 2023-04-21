#include "DelayControls.h"

// This component has 4 controls in total. First, a checkbox that activates or deactivates bpm sync (ID: "TempoSync") and a Feedback slider.
// Then, if the TempoSync parameter is on, the third parameter will be a combobox that lets the user choice delay values based on the bpm
// (1/2,1/4,3/4,1/1...). In the other hand, if TempoSync is off, the third parameter will be a slider to select a concrete delay time. 
// Turn visible/invisible the other third component as necessary.

DelayControls::DelayControls(HandyFXAudioProcessor& p)
    : audioProcessor(p),
    tempoSyncButton(std::make_unique<juce::ToggleButton>("TempoSync")),
    feedbackSlider(std::make_unique<juce::Slider>("Feedback")),
    delaySlider(std::make_unique<juce::Slider>("Delay")),
    delayDivComboBox(std::make_unique<juce::ComboBox>("DelayDiv")),
    feedbackLabel(std::make_unique<juce::Label>("FeedbackLabel", "Feedback")),
    delayLabel(std::make_unique<juce::Label>("DelayLabel", "Delay"))
{
    // Create the controls
    tempoSyncButton->setButtonText("Tempo Sync");
    tempoSyncButton->setToggleState(false, juce::dontSendNotification);
    tempoSyncButton->addListener(this);
    addAndMakeVisible(tempoSyncButton.get());

    feedbackSlider->setRange(0.1f, 10.f, 0.05f);
    feedbackSlider->setValue(0.7f);
    feedbackSlider->addListener(this);
    addAndMakeVisible(feedbackSlider.get());

    delaySlider->setRange(0.01f, 2.f, 0.01f);
    delaySlider->setValue(1.f);
    delaySlider->addListener(this);
    addAndMakeVisible(delaySlider.get());

    addAndMakeVisible(delayDivComboBox.get());
    delayDivComboBox->addItem("1/8", 1);
    delayDivComboBox->addItem("1/4", 2);
    delayDivComboBox->addItem("1/2", 3);
    delayDivComboBox->addItem("3/4", 4);
    delayDivComboBox->addItem("1/1", 5);
    delayDivComboBox->addItem("2/1", 6);
    delayDivComboBox->addListener(this);
    delayDivComboBox->setSelectedId(1);
    
    //add text to the combo box "Delay"
    delayDivComboBox->setJustificationType(juce::Justification::centred);
    delayDivComboBox->setTextWhenNothingSelected("1/8");
    delayDivComboBox->setTextWhenNoChoicesAvailable("No choices available");

    // Add labels
    addAndMakeVisible(feedbackLabel.get());
    feedbackLabel->setText("Feedback", juce::dontSendNotification);
    feedbackLabel->attachToComponent(feedbackSlider.get(), true);

    addAndMakeVisible(delayLabel.get());
    delayLabel->setText("Time", juce::dontSendNotification);
    delayLabel->attachToComponent(delaySlider.get(), true);

    // Set the controls' values
    tempoSyncButton->setToggleState(audioProcessor.parameters.getRawParameterValue("TempoSync")->load(), juce::dontSendNotification);
    feedbackSlider->setValue(audioProcessor.parameters.getRawParameterValue("Feedback")->load(), juce::dontSendNotification);
    delaySlider->setValue(audioProcessor.parameters.getRawParameterValue("Delay")->load(), juce::dontSendNotification);
    delayDivComboBox->setSelectedId(audioProcessor.parameters.getRawParameterValue("DelayDiv")->load(), juce::dontSendNotification);

    // Set the controls' visibility
    if (tempoSyncButton->getToggleState())
    {
        delaySlider->setVisible(false);
        delayDivComboBox->setVisible(true);
    }
    else
    {
        delaySlider->setVisible(true);
        delayDivComboBox->setVisible(false);
    }
}

DelayControls::~DelayControls()
{
}

void DelayControls::paint(juce::Graphics& g)
{
}

void DelayControls::resized()
{
	juce::Rectangle<int> area = getLocalBounds();
	juce::Rectangle<int> tempoSyncArea = area.reduced(20);
	juce::Rectangle<int> feedbackArea = area.removeFromTop(20);
    feedbackArea.removeFromLeft(80);
    juce::Rectangle<int> delayArea = area.removeFromTop(20);
    delayArea.removeFromLeft(80);
    juce::Rectangle<int> delayDivArea = area.removeFromTop(20);
    delayDivArea.removeFromRight(80);
	tempoSyncButton->setBounds(tempoSyncArea);
	feedbackSlider->setBounds(feedbackArea);
	delaySlider->setBounds(delayArea);
	delayDivComboBox->setBounds(delayDivArea);
}

void DelayControls::sliderValueChanged(juce::Slider* slider)
{
	if (slider == feedbackSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("Feedback")->store(slider->getValue());
	}
	else if (slider == delaySlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("Delay")->store(slider->getValue());
	}
}

void DelayControls::comboBoxChanged(juce::ComboBox* comboBox)
{
	if (comboBox == delayDivComboBox.get())
	{
		audioProcessor.parameters.getRawParameterValue("DelayDiv")->store(comboBox->getSelectedId());
	}
}

void DelayControls::buttonClicked(juce::Button* button)
{
	if (button == tempoSyncButton.get())
	{
		audioProcessor.parameters.getRawParameterValue("TempoSync")->store(button->getToggleState());
		if (button->getToggleState())
		{
			delaySlider->setVisible(false);
			delayDivComboBox->setVisible(true);
            delayDivComboBox->setSelectedId(1);
		}
		else
		{
			delaySlider->setVisible(true);
			delayDivComboBox->setVisible(false);
		}
	}
}

