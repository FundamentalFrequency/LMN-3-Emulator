#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "Encoder.h"
#include "ButtonKeyboard.h"

class MainComponent : public juce::Component, 
    public juce::Button::Listener,
    public Encoder::Listener,
    public juce::Timer,
    private juce::MidiKeyboardStateListener {
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void encoderDoubleClicked(Encoder* encoder) override;
    void encoderIncreased(Encoder* encoder) override;
    void encoderDecreased(Encoder* encoder) override;

    void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

private:
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
    const double JOYSTICK_LEFT_EDGE_PADDING = 0;
    const double JOYSTICK_RIGHT_EDGE_PADDING = HORIZONTAL_KEY_SPACING;
    const double JOYSTICK_BOTTOM_EDGE_PADDING	= 0;
    const double ENCODER_SPACING = 7;

    const int NUM_COLS = 16;
    const int NUM_ROWS = 5;
    const double DEVICE_WIDTH = (NUM_COLS * KEY_WIDTH) + ((NUM_COLS - 1) * HORIZONTAL_KEY_SPACING)  + LEFT_EDGE_PADDING + RIGHT_EDGE_PADDING;
    const double DEVICE_HEIGHT = (NUM_ROWS * KEY_HEIGHT) + ((NUM_ROWS - 1) * VERTICAL_KEY_SPACING) + TOP_EDGE_PADDING + BOTTOM_EDGE_PADDING;
    
    std::unique_ptr<juce::MidiOutput> defaultMIDIOutput;
    double startTime;

    juce::OwnedArray<juce::TextButton> row0;
    juce::OwnedArray<juce::TextButton> row1;
    juce::OwnedArray<juce::TextButton> row2;
    juce::OwnedArray<juce::TextButton> row3;
    juce::OwnedArray<juce::TextButton> row4;

    juce::OwnedArray<Encoder> encoders;

    juce::Slider joystick;

    juce::MidiKeyboardState keyboardState;         
    ButtonKeyboard keyboardComponent;  

    juce::TextEditor midiMessagesBox;

    juce::AudioDeviceManager deviceManager;   
    int lastOutputIndex = 0;                  

    juce::ComboBox midiOutputList;                
    juce::Label midiOutputListLabel;

    void initializeRow0Buttons();
    void setRow0ButtonsBounds();

    void initializeRow1Buttons();
    void setRow1ButtonsBounds();

    void initializeRow2Buttons();
    void setRow2ButtonsBounds();

    void initializeRow3Buttons();
    void setRow3ButtonsBounds();

    void initializeRow4Buttons();
    void setRow4ButtonsBounds();

    void initializeEncoders();
    void setEncodersBounds();

    void setJoystickBounds();

    void initializeKeyboard();
    void setMidiKeyboardBounds();

    void initializeMidiMessagesBox();
    void setMidiMessagesBoxBounds();

    void initializeMidiOutput();
    void setMidiOutput(int index);

    void sendCCMessage(int channel, int type, int value);

    void removeListenersFromRow(juce::OwnedArray<juce::TextButton>* row);

    void timerCallback();

    static juce::String getMidiMessageDescription(const juce::MidiMessage& m);

    void logMessage(const juce::String& m);

    void addMessageToList(const juce::MidiMessage& message, const juce::String& source);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};