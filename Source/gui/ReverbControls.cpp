
#include "ReverbControls.h"

ReverbControls::ReverbControls(HandyFXAudioProcessor& p) : audioProcessor(p),
	roomSizeSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	dampingSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	widthSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	wetLevelSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)),
	dryLevelSlider(std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow))

{
	
	// Create the sliders
	roomSizeSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	roomSizeSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 20);
	roomSizeSlider->setRange(0.0f, 1.0f, 0.01f);
	roomSizeSlider->addListener(this);

	dampingSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	dampingSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 20);
	dampingSlider->setRange(0.0f, 1.0f, 0.01f);
	dampingSlider->addListener(this);

	widthSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	widthSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 20);
	widthSlider->setRange(0.0f, 1.0f, 0.01f);
	widthSlider->addListener(this);

	wetLevelSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	wetLevelSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 20);
	wetLevelSlider->setRange(0.0f, 1.0f, 0.01f);
	wetLevelSlider->addListener(this);

	dryLevelSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	dryLevelSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 20);
	dryLevelSlider->setRange(0.0f, 1.0f, 0.01f);
	dryLevelSlider->addListener(this);

	// Set the sliders values
	roomSizeSlider->setValue(*p.parameters.getRawParameterValue("RoomSize"));
	dampingSlider->setValue(*p.parameters.getRawParameterValue("Damping"));
	widthSlider->setValue(*p.parameters.getRawParameterValue("Width"));
	wetLevelSlider->setValue(*p.parameters.getRawParameterValue("WetLevel"));
	dryLevelSlider->setValue(*p.parameters.getRawParameterValue("DryLevel"));

	// Set the sliders' labels
	roomSizeSlider->setTextValueSuffix(" Room Size");
	dampingSlider->setTextValueSuffix(" Damping");
	widthSlider->setTextValueSuffix(" Width");
	wetLevelSlider->setTextValueSuffix(" Wet Level");
	dryLevelSlider->setTextValueSuffix(" Dry Level");

	// Add the sliders to the component
	addAndMakeVisible(roomSizeSlider.get());
	addAndMakeVisible(dampingSlider.get());
	addAndMakeVisible(widthSlider.get());
	addAndMakeVisible(wetLevelSlider.get());
	addAndMakeVisible(dryLevelSlider.get());

}

ReverbControls::~ReverbControls()
{
}

void ReverbControls::paint(juce::Graphics& g)
{
	//// (Our component is opaque, so we must completely fill the background with a solid colour)
	//g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	//// Draw the background
	//g.setColour(juce::Colours::black);
	//g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
	//g.setColour(juce::Colours::white);
	//g.setFont(15.0f);
	//g.drawText("Reverb Controls", getLocalBounds(),
	//			juce::Justification::centred, true);   // draw some placeholder text
}

void ReverbControls::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
	// Set the sliders' bounds
	roomSizeSlider->setBounds(10, 30, 100, 100);
	dampingSlider->setBounds(120, 30, 100, 100);
	widthSlider->setBounds(230, 30, 100, 100);
	wetLevelSlider->setBounds(340, 30, 100, 100);
	dryLevelSlider->setBounds(450, 30, 100, 100);
}

void ReverbControls::sliderValueChanged(juce::Slider* slider)
{
	// Set the sliders' values
	if (slider == roomSizeSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("RoomSize")->store(roomSizeSlider->getValue());
	}
	else if (slider == dampingSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("Damping")->store(dampingSlider->getValue());
	}
	else if (slider == widthSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("Width")->store(widthSlider->getValue());
	}
	else if (slider == wetLevelSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("WetLevel")->store(wetLevelSlider->getValue());
	}
	else if (slider == dryLevelSlider.get())
	{
		audioProcessor.parameters.getRawParameterValue("DryLevel")->store(dryLevelSlider->getValue());
	}
}

