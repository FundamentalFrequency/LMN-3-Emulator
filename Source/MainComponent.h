#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public juce::Component, public juce::Button::Listener
{
public:
    //==============================================================================
    MainComponent();

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;

private:
    //==============================================================================
    // Your private member variables go here...
    const double PX_TO_MM = 4;
    const double TOP_EDGE_PADDING = 10.4775;
    const double LEFT_EDGE_PADDING = TOP_EDGE_PADDING;
    const double RIGHT_EDGE_PADDING = TOP_EDGE_PADDING;
    const double BOTTOM_EDGE_PADDING = TOP_EDGE_PADDING;
    const double KEY_WIDTH = 14;
    const double KEY_HEIGHT = KEY_WIDTH;
    const double HORIZONTAL_KEY_SPACING = 5.05;
    const double VERTICAL_KEY_SPACING = HORIZONTAL_KEY_SPACING;

    const double HYPERPIXEL_WIDTH = 98;
    const double HYPERPIXEL_HEIGHT = 58.2;
    const double HYPERPIXEL_SCREEN_WIDTH = (HYPERPIXEL_WIDTH - 6);

    const double JOYSTICK_TOP_EDGE_PADDING = VERTICAL_KEY_SPACING;
    const double JOYSTICK_LEFT_EDGE_PADDING = 0 * PX_TO_MM;
    const double JOYSTICK_RIGHT_EDGE_PADDING = HORIZONTAL_KEY_SPACING;
    const double JOYSTICK_BOTTOM_EDGE_PADDING	= 0;
    const double ENCODER_SPACING = 7;

    const int NUM_COLS = 16;
    const int NUM_ROWS = 5;
    const double DEVICE_WIDTH = (NUM_COLS * KEY_WIDTH) + ((NUM_COLS - 1) * HORIZONTAL_KEY_SPACING)  + LEFT_EDGE_PADDING + RIGHT_EDGE_PADDING;
    const double DEVICE_HEIGHT = (NUM_ROWS * KEY_HEIGHT) + ((NUM_ROWS - 1) * VERTICAL_KEY_SPACING) + TOP_EDGE_PADDING + BOTTOM_EDGE_PADDING;
    
    std::unique_ptr<juce::MidiOutput> defaultMIDIOutput;
    double startTime;
    juce::TextButton testButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};