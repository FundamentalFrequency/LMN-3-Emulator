#include "EmulatorLookAndFeel.h"
#include "ButtonKeyboard.h"

EmulatorLookAndFeel::EmulatorLookAndFeel()
{

    setColour(juce::DocumentWindow::backgroundColourId, backgroundColour);

    setColour(juce::Label::textColourId, textColour);   

    setColour(juce::ComboBox::backgroundColourId, backgroundColour);
    // This also controls the TextButton outline color for some reason...
    setColour(juce::ComboBox::outlineColourId, textColour);

    setColour(juce::PopupMenu::backgroundColourId, backgroundColour);
    setColour(juce::PopupMenu::highlightedTextColourId, backgroundColour);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, textColour);


    setColour(juce::ScrollBar::thumbColourId, blueColour);
    setColour(juce::ScrollBar::trackColourId, blueColour);

    setColour(juce::TextEditor::outlineColourId, textColour);
    setColour(juce::TextEditor::backgroundColourId, backgroundColour);
    setColour(juce::TextEditor::shadowColourId, backgroundColour);
    setColour(juce::TextEditor::textColourId, textColour);

    setColour(juce::TextButton::buttonColourId, blackColour);
    setColour(juce::TextButton::textColourOnId, redColour);
    setColour(juce::TextButton::textColourOffId, textColour);

    setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, yellowColour.withMultipliedAlpha(.75));
    setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, yellowColour);
    setColour(ButtonKeyboard::buttonOutlineColourId, whiteColour);
}

juce::Font EmulatorLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight) {
    fontAwesomeFont.setHeight(juce::jmin(16.0f, (float) buttonHeight * 0.6f));
    return fontAwesomeFont;
}