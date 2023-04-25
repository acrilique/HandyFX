
#include "VibratoControls.h"
// Set chorus effect as a vibrato (mix to 1)
VibratoControls::VibratoControls(HandyFXAudioProcessor& p) : audioProcessor(p),
	rateSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	depthSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	delaySlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	feedbackSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	mixSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	vibratoButton(std::make_unique<juce::ToggleButton>("Vibrato")),
	chorusButton(std::make_unique<juce::ToggleButton>("Chorus"))

{
	// Rate slider
	rateSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	rateSlider->setRange(0.0f, 100.0f);
	rateSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
	rateSlider->addListener(this);
	addAndMakeVisible(rateSlider.get());

	// Depth slider
	depthSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	depthSlider->setRange(0.0f, 1.0f);
	depthSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
	depthSlider->addListener(this);
	addAndMakeVisible(depthSlider.get());

	// Delay slider
	delaySlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	delaySlider->setRange(0.0f, 100.0f);
	delaySlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
	delaySlider->addListener(this);
	addAndMakeVisible(delaySlider.get());

	// Feedback slider
	feedbackSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	feedbackSlider->setRange(-1.0f, 1.0f);
	feedbackSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
	feedbackSlider->addListener(this);
	addAndMakeVisible(feedbackSlider.get());

	// Mix slider
	mixSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	mixSlider->setRange(0.0f, 1.0f);
	mixSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
	mixSlider->addListener(this);
	addAndMakeVisible(mixSlider.get());

	// Vibrato button
	vibratoButton->setRadioGroupId(1);
	vibratoButton->setButtonText("Vibrato");
	vibratoButton->setToggleState(true, juce::NotificationType::dontSendNotification);
	vibratoButton->addListener(this);
	addAndMakeVisible(vibratoButton.get());

	// Chorus button
	chorusButton->setRadioGroupId(1);
	chorusButton->setButtonText("Chorus");
	chorusButton->setToggleState(false, juce::NotificationType::dontSendNotification);
	chorusButton->addListener(this);
	addAndMakeVisible(chorusButton.get());

	// Set values
	rateSlider->setValue(p.parameters.getRawParameterValue("Rate")->load());
	depthSlider->setValue(p.parameters.getRawParameterValue("Depth")->load());
	delaySlider->setValue(p.parameters.getRawParameterValue("CentreDelay")->load());
	feedbackSlider->setValue(p.parameters.getRawParameterValue("VibratoFeedback")->load());
	mixSlider->setValue(p.parameters.getRawParameterValue("Mix")->load());

	// Set the sliders labels
	rateSlider->setTextValueSuffix(" Hz");
	delaySlider->setTextValueSuffix(" ms");
	mixSlider->setTextValueSuffix(" %");

	// Set default effect
	vibratoButton->setToggleState(true, juce::NotificationType::dontSendNotification);
		
}

VibratoControls::~VibratoControls()
{
}

void VibratoControls::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	//g.setColour(juce::Colours::white);
	//g.setFont(14.0f);
	//g.drawText("VibratoControls", getLocalBounds(), juce::Justification::centred, true);
}

void VibratoControls::resized()
{
	rateSlider->setBounds(10, 240, 120, 120);
	depthSlider->setBounds(230, 240, 120, 120);
	delaySlider->setBounds(450, 240, 120, 120);
	feedbackSlider->setBounds(120, 0, 120, 120);
	mixSlider->setBounds(340, 0, 120, 120);
}

void VibratoControls::sliderValueChanged(juce::Slider* slider)
{
	if (slider == rateSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("Rate")->store(rateSlider->getValue());
	}
	else if (slider == depthSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("Depth")->store(depthSlider->getValue());
	}
	else if (slider == delaySlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("CentreDelay")->store(delaySlider->getValue());
	}
	else if (slider == feedbackSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("VibratoFeedback")->store(feedbackSlider->getValue());
	}
	else if (slider == mixSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("Mix")->store(mixSlider->getValue());
	}
}

void VibratoControls::buttonClicked(juce::Button* button)
{
	if (button == vibratoButton.get())
	{
		audioProcessor.parameters.getRawParameterValue("Mix")->store(1.0f);
		mixSlider->setVisible(false);

	}
	else if (button == chorusButton.get())
	{
		audioProcessor.parameters.getRawParameterValue("Mix")->store(0.5f);
		mixSlider->setVisible(true);
	}
}