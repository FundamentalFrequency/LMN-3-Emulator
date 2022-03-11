#include "ButtonKeyboard.h"

ButtonKeyboard::ButtonKeyboard(juce::MidiKeyboardState &s, juce::MidiKeyboardComponent::Orientation o) : MidiKeyboardComponent(s, o) {
}

ButtonKeyboard::~ButtonKeyboard() {
}

void ButtonKeyboard::drawWhiteNote(int midiNoteNumber,
                                   juce::Graphics &g,
                                   juce::Rectangle<float> area,
                                   bool isDown,
                                   bool isOver,
                                   juce::Colour lineColour,
                                   juce::Colour  textColour) {
    auto colour = juce::Colours::blueviolet;
    if (isDown) colour = findColour(keyDownOverlayColourId);
    if (isOver) colour = colour.overlaidWith(findColour(mouseOverKeyOverlayColourId));
    g.setColour(colour);
    area.setY(area.getY() + (area.getHeight() / 2.0));
    area.setHeight(area.getHeight() / 2.0);
    area.reduce(5.05 * 4 / 2.0, 5.05 * 4 / 2.0);
    g.fillRoundedRectangle(area, 10.0);
}

void ButtonKeyboard::drawBlackNote(int midiNoteNumber,
                                   juce::Graphics &g,
                                   juce::Rectangle<float> area,
                                   bool isDown,
                                   bool isOver,
                                   juce::Colour  textColour) {
    auto colour = juce::Colours::blueviolet;
    if (isDown) colour = findColour(keyDownOverlayColourId);
    if (isOver) colour = colour.overlaidWith(findColour(mouseOverKeyOverlayColourId));
    g.setColour(colour);
    area.reduce(5.05 * 4 / 2.0, 5.05 * 4 / 2.0);
    g.fillRoundedRectangle(area, 10.0);
}