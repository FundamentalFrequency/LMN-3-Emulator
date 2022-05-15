#pragma once

#include "ButtonKeyboard.h"
#include "EmulatorLookAndFeel.h"
#include "Encoder.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class MainComponent : public juce::Component,
                      public juce::Button::Listener,
                      public Encoder::Listener,
                      public juce::Slider::Listener,
                      public juce::Timer,
                      private juce::MidiInputCallback,
                      private juce::MidiKeyboardStateListener {
  public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;
    void buttonClicked(juce::Button *button) override;
    void buttonStateChanged(juce::Button *button) override;
    void encoderDoubleClicked(Encoder *encoder) override;
    void encoderIncreased(Encoder *encoder) override;
    void encoderDecreased(Encoder *encoder) override;

    void handleNoteOn(juce::MidiKeyboardState *, int midiChannel,
                      int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState *, int midiChannel,
                       int midiNoteNumber, float velocity) override;

    void handleIncomingMidiMessage(juce::MidiInput *source,
                                   const juce::MidiMessage &message) override;

    void modifierKeysChanged(const juce::ModifierKeys &modifiers) override;

    void sliderDragEnded(juce::Slider *slider) override;
    void sliderValueChanged(juce::Slider *slider) override;

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
    const double JOYSTICK_BOTTOM_EDGE_PADDING = 0;
    const double ENCODER_SPACING = 7;

    const int NUM_COLS = 16;
    const int NUM_ROWS = 5;
    const double DEVICE_WIDTH = (NUM_COLS * KEY_WIDTH) +
                                ((NUM_COLS - 1) * HORIZONTAL_KEY_SPACING) +
                                LEFT_EDGE_PADDING + RIGHT_EDGE_PADDING;
    const double DEVICE_HEIGHT = (NUM_ROWS * KEY_HEIGHT) +
                                 ((NUM_ROWS - 1) * VERTICAL_KEY_SPACING) +
                                 TOP_EDGE_PADDING + BOTTOM_EDGE_PADDING;

    const int MIDI_MESSAGE_BOX_MESSAGE_LIMIT = 100;

    const int OCTAVE_MAX = 4;
    const int OCTAVE_MIN = -4;

    EmulatorLookAndFeel lookAndFeel;

    std::unique_ptr<juce::MidiOutput> defaultMIDIOutput;
    double startTime;

    juce::OwnedArray<juce::TextButton> row0;
    juce::OwnedArray<juce::TextButton> row1;
    juce::OwnedArray<juce::TextButton> row2;
    juce::OwnedArray<juce::TextButton> row3;
    juce::OwnedArray<juce::TextButton> row4;

    std::map<juce::String, juce::Button::ButtonState> previousButtonStates;

    juce::StringArray row0Icons = {
        juce::String::charToString(0xf363), juce::String::charToString(0xf08b),
        juce::String::charToString(0xf090), juce::String::charToString(0x2b),
        juce::String::charToString(0xf068)};

    juce::StringArray row1Icons = {
        juce::String::charToString(0xf0c4), juce::String::charToString(0xf0ea),
        juce::String::charToString(0xe568), juce::String::charToString(0xf0c7),
        juce::String::charToString(0xf2ea)};

    juce::StringArray row2Icons = {
        juce::String::charToString(0xf094), juce::String::charToString(0xf8d9),
        juce::String::charToString(0xf04b), juce::String::charToString(0xf04d),
        juce::String::charToString(0xf013), juce::String::charToString(0xf017),
        juce::String::charToString(0xe473), juce::String::charToString(0xe0e4),
        juce::String::charToString(0xf1e6), juce::String::charToString(0xf83e),
        juce::String::charToString(0xf00a)};

    juce::OwnedArray<Encoder> encoders;

    juce::Slider joystick;

    juce::MidiKeyboardState keyboardState;
    ButtonKeyboard keyboardComponent;

    juce::TextEditor midiMessagesBox;
    int numMessagesInEditor = 0;

    juce::AudioDeviceManager deviceManager;

    int lastOutputIndex = 0;
    juce::ComboBox midiOutputList;
    juce::Label midiOutputListLabel;

    int lastInputIndex = 0;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;

    bool isAddingFromMidiInput = false;

    bool isCtrlDown = false;
    bool isPlusDown = false;
    bool isMinusDown = false;
    int currentOctave = 0;

    // This is used to dispach an incoming message to the message thread
    class IncomingMessageCallback : public juce::CallbackMessage {
      public:
        IncomingMessageCallback(MainComponent *o, const juce::MidiMessage &m,
                                const juce::String &s)
            : owner(o), message(m), source(s) {}

        void messageCallback() override {
            if (owner != nullptr) {
                owner->addMessageToList(message, source);
            }
        }

        Component::SafePointer<MainComponent> owner;
        juce::MidiMessage message;
        juce::String source;
    };

    void configureButton(juce::TextButton *button);

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

    void initializeJoystick();
    void setJoystickBounds();

    void initializeKeyboard();
    void setMidiKeyboardBounds();

    void initializeMidiMessagesBox();
    void setMidiMessagesBoxBounds();

    void initializeMidiOutput();
    void setMidiOutput(int index);
    void initializeMidiInput();
    void setMidiInput(int index);
    void sendCCMessage(int channel, int type, int value);
    void addMessageToList(const juce::MidiMessage &message,
                          const juce::String &source);
    void postMessageToList(const juce::MidiMessage &message,
                           const juce::String &source);

    void removeListenersFromRow(juce::OwnedArray<juce::TextButton> *row);

    void timerCallback() override;

    static juce::String getMidiMessageDescription(const juce::MidiMessage &m);

    void logMessage(const juce::String &m);

    int getCCValueForCurrentOctave();
    void increaseOctave();
    void decreaseOctave();

    bool keyStateChanged(bool isKeyDown) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};