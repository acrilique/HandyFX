#pragma once

#include <JuceHeader.h>

class ComboBoxWithLabel : public juce::Component,
    public juce::ComboBox::Listener
{
public:
    ComboBoxWithLabel();

    void resized() override;

private:
    juce::ComboBox comboBox;
    juce::Label label;
};
