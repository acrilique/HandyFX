
#include "ComboBoxWithLabel.h"

ComboBoxWithLabel::ComboBoxWithLabel()
{
	addAndMakeVisible(comboBox);
	addAndMakeVisible(label);
	label.setText("ComboBox Label", juce::dontSendNotification);
	comboBox.addItem("1/8", 1);
	comboBox.addItem("1/4", 2);
	comboBox.addItem("1/2", 3);
	comboBox.addItem("3/4", 4);
	comboBox.addItem("1/1", 5);
	comboBox.addItem("2/1", 6);
	delayDivComboBox->setSelectedId(1);

	comboBox.addListener(this);
}

void ComboBoxWithLabel::resized()
{
	auto bounds = getLocalBounds();
	label.setBounds(bounds.removeFromTop(20));
	comboBox.setBounds(bounds);
}