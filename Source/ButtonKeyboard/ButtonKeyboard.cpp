#include "ButtonKeyboard.h"

ButtonKeyboard::ButtonKeyboard(juce::MidiKeyboardState &s,
                               juce::MidiKeyboardComponent::Orientation o)
    : MidiKeyboardComponent(s, o) {}

ButtonKeyboard::~ButtonKeyboard() {}

void ButtonKeyboard::drawWhiteNote(int /*midiNoteNumber*/, juce::Graphics &g,
                                   juce::Rectangle<float> area, bool isDown,
                                   bool isOver, juce::Colour /*lineColour*/,
                                   juce::Colour /*textColour*/) {
    auto colour = findColour(juce::TextButton::buttonColourId);
    if (isDown)
        colour = findColour(keyDownOverlayColourId);
    if (isOver)
        colour = colour.overlaidWith(findColour(mouseOverKeyOverlayColourId));
    g.setColour(colour);
    area.setY(static_cast<float>(area.getY() + (area.getHeight() / 2.0)));
    area.setHeight(static_cast<float>(area.getHeight() / 2.0));
    area.reduce(static_cast<float>(5.05 * 4 / 2.0),
                static_cast<float>(5.05 * 4 / 2.0));
    g.fillRect(area);

    // Draw button outline
    colour = findColour(buttonOutlineColourId);
    g.setColour(colour);
    g.drawRect(area);
}

void ButtonKeyboard::drawBlackNote(int /*midiNoteNumber*/, juce::Graphics &g,
                                   juce::Rectangle<float> area, bool isDown,
                                   bool isOver, juce::Colour /*textColour*/) {
    auto colour = findColour(juce::TextButton::buttonColourId);
    if (isDown)
        colour = findColour(keyDownOverlayColourId);
    if (isOver)
        colour = colour.overlaidWith(findColour(mouseOverKeyOverlayColourId));
    g.setColour(colour);
    area.reduce(static_cast<float>(5.05 * 4 / 2.0),
                static_cast<float>(5.05 * 4 / 2.0));
    g.fillRect(area);

    // Draw button outline
    colour = findColour(buttonOutlineColourId);
    g.setColour(colour);
    g.drawRect(area);
}