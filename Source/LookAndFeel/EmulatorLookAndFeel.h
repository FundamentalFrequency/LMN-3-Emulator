#pragma once
#include "FontData.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

class EmulatorLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    EmulatorLookAndFeel();
    juce::Font getTextButtonFont(juce::TextButton &, int buttonHeight) override;

    juce::Typeface::Ptr fontAwesomeTypeface =
        juce::Typeface::createSystemTypefaceFor(
            FontData::FontAwesome6FreeSolid900_otf,
            FontData::FontAwesome6FreeSolid900_otfSize);
    juce::Font fontAwesomeFont = juce::Font(fontAwesomeTypeface);

    juce::Colour blueColour = juce::Colour(0xff458588);
    juce::Colour greenColour = juce::Colour(0xff689d6a);
    juce::Colour whiteColour = juce::Colour(0xfff9f5d7);
    juce::Colour redColour = juce::Colour(0xffcc241d);
    juce::Colour yellowColour = juce::Colour(0xffd79921);
    juce::Colour orangeColour = juce::Colour(0xffd65d0e);
    juce::Colour purpleColour = juce::Colour(0xffb16286);
    juce::Colour blackColour = juce::Colour(0xff282828);
    juce::Colour backgroundColour = juce::Colour(0xff1d2021);
    juce::Colour textColour = juce::Colour(0xfff9f5d7);
    juce::Colour colour1 = blueColour;
    juce::Colour colour2 = greenColour;
    juce::Colour colour3 = whiteColour;
    juce::Colour colour4 = redColour;
};
