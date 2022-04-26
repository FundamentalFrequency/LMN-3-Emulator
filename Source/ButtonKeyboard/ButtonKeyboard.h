#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class ButtonKeyboard : public juce::MidiKeyboardComponent {
  public:
    ButtonKeyboard(juce::MidiKeyboardState &state,
                   juce::MidiKeyboardComponent::Orientation orientation);
    ~ButtonKeyboard() override;

    enum ButtonKeyboardColourIds { buttonOutlineColourId = uint32_t(0) };

    void drawWhiteNote(int midiNoteNumber, juce::Graphics &g,
                       juce::Rectangle<float> area, bool isDown, bool isOver,
                       juce::Colour lineColour,
                       juce::Colour textColour) override;

    void drawBlackNote(int midiNoteNumber, juce::Graphics &g,
                       juce::Rectangle<float> area, bool isDown, bool isOver,
                       juce::Colour textColour) override;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonKeyboard)
};