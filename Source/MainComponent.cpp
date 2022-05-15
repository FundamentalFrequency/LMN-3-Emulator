#include "MainComponent.h"
#include "MidiMappings.h"

MainComponent::MainComponent()
    : defaultMIDIOutput(juce::MidiOutput::openDevice(
          juce::MidiOutput::getDefaultDevice().identifier)),
      startTime(juce::Time::getMillisecondCounterHiRes() * 0.001),
      keyboardComponent(keyboardState,
                        juce::MidiKeyboardComponent::horizontalKeyboard) {
    DBG("Initialized with default Midi Device: " +
        defaultMIDIOutput->getDeviceInfo().name);

    setLookAndFeel(&lookAndFeel);

    initializeRow0Buttons();
    initializeRow1Buttons();
    initializeRow2Buttons();
    initializeRow3Buttons();
    initializeRow4Buttons();

    initializeEncoders();

    initializeJoystick();

    initializeKeyboard();

    initializeMidiMessagesBox();
    initializeMidiOutput();
    initializeMidiInput();

    setSize(DEVICE_WIDTH * PX_TO_MM, DEVICE_HEIGHT * PX_TO_MM);
}

MainComponent::~MainComponent() {
    removeListenersFromRow(&row0);
    removeListenersFromRow(&row1);
    removeListenersFromRow(&row2);

    for (auto *encoder : encoders) {
        encoder->removeListener(this);
    }

    keyboardState.removeListener(this);

    joystick.removeListener(this);
}

void MainComponent::timerCallback() {
    stopTimer();
    keyboardComponent.grabKeyboardFocus();
}

void MainComponent::removeListenersFromRow(
    juce::OwnedArray<juce::TextButton> *row) {
    for (auto *button : *row) {
        button->removeListener(this);
    }
}

void MainComponent::configureButton(juce::TextButton *button) {
    button->setConnectedEdges(juce::TextButton::ConnectedOnLeft |
                              juce::TextButton::ConnectedOnRight);
    button->setWantsKeyboardFocus(false);
}

void MainComponent::initializeRow0Buttons() {
    for (int i = 5; i <= 9; i++) {
        auto *button = new juce::TextButton();
        configureButton(button);
        row0.add(button);
        button->addListener(this);
        button->setName("R0C" + std::to_string(i));
        previousButtonStates.insert(std::pair<juce::String, juce::Button::ButtonState>(button->getName(),
                                                                                       button->getState()));
        juce::String buttonText = row0Icons[i - 5];
        button->setButtonText(buttonText);
        addAndMakeVisible(button);
    }
}

void MainComponent::initializeRow1Buttons() {
    for (int i = 5; i <= 9; i++) {
        auto *button = new juce::TextButton();
        configureButton(button);
        row1.add(button);
        button->addListener(this);
        button->setName("R1C" + std::to_string(i));
        previousButtonStates.insert(std::pair<juce::String, juce::Button::ButtonState>(button->getName(),
                                                                                       button->getState()));
        juce::String buttonText = row1Icons[i - 5];
        button->setButtonText(buttonText);
        addAndMakeVisible(button);
    }
}

void MainComponent::initializeRow2Buttons() {
    for (int i = 5; i <= 15; i++) {
        auto *button = new juce::TextButton();
        configureButton(button);
        row2.add(button);
        button->addListener(this);
        button->setName("R2C" + std::to_string(i));
        previousButtonStates.insert(std::pair<juce::String, juce::Button::ButtonState>(button->getName(),
                                                                                       button->getState()));
        juce::String buttonText = row2Icons[i - 5];
        button->setButtonText(buttonText);
        addAndMakeVisible(button);
    }
}

void MainComponent::initializeRow3Buttons() {
    for (int i = 3; i <= 15; i++) {
        if (i != 6 && i != 9 && i != 13) {
            auto *button = new juce::TextButton();
            configureButton(button);
            row3.add(button);
            button->addListener(this);
            button->setName("R3C" + std::to_string(i));
            addAndMakeVisible(button);
        }
    }
}

void MainComponent::initializeRow4Buttons() {
    for (int i = 2; i <= 15; i++) {
        auto *button = new juce::TextButton();
        configureButton(button);
        row4.add(button);
        button->addListener(this);
        button->setName("R4C" + std::to_string(i));
        button->setButtonText("R4C" + std::to_string(i));
        addAndMakeVisible(button);
    }
}

void MainComponent::initializeEncoders() {
    for (int i = 1; i <= 4; i++) {
        auto *encoder = new Encoder();
        encoders.add(encoder);
        encoder->setName("encoder" + std::to_string(i));
        encoder->addListener(this);
        juce::Colour encoderColour;
        switch (i) {
        case 1:
            encoderColour = lookAndFeel.blueColour;
            break;
        case 2:
            encoderColour = lookAndFeel.greenColour;
            break;
        case 3:
            encoderColour = lookAndFeel.whiteColour;
            break;
        case 4:
            encoderColour = lookAndFeel.redColour;
            break;
        default:
            encoderColour = lookAndFeel.blueColour;
        }
        encoder->setColour(Encoder::thumbColourId, encoderColour);
        encoder->setColour(Encoder::rotarySliderFillColourId,
                           encoderColour.darker(.7f));
        encoder->setColour(Encoder::rotarySliderOutlineColourId,
                           encoderColour.darker(.7f));
        addAndMakeVisible(encoder);
    }
}

void MainComponent::initializeJoystick() {
    joystick.setSliderStyle(
        juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    joystick.setScrollWheelEnabled(false);
    joystick.setRange(0, 16383, 1);
    double midpoint = joystick.getRange().getLength() / 2;
    joystick.setValue(midpoint, juce::dontSendNotification);
    joystick.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox,
                             true, 0, 0);
    joystick.setColour(juce::Slider::thumbColourId, lookAndFeel.whiteColour);
    joystick.setColour(juce::Slider::rotarySliderFillColourId,
                       lookAndFeel.whiteColour.darker(.7f));
    joystick.setColour(juce::Slider::rotarySliderOutlineColourId,
                       lookAndFeel.whiteColour.darker(.7f));
    joystick.addListener(this);
    addAndMakeVisible(joystick);
}

void MainComponent::initializeKeyboard() {
    keyboardComponent.setWantsKeyboardFocus(true);
    keyboardComponent.setColour(
        juce::MidiKeyboardComponent::whiteNoteColourId,
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    keyboardComponent.setColour(
        juce::MidiKeyboardComponent::shadowColourId,
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    keyboardComponent.setColour(
        juce::MidiKeyboardComponent::keySeparatorLineColourId,
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    keyboardComponent.setMidiChannel(MIDI_CHANNEL);
    keyboardComponent.setLowestVisibleKey(53);
    keyboardComponent.setAvailableRange(53, 76);
    keyboardComponent.setBlackNoteLengthProportion(.5);
    keyboardComponent.setBlackNoteWidthProportion(1.0);
    keyboardComponent.setKeyWidth((KEY_WIDTH + HORIZONTAL_KEY_SPACING) *
                                  PX_TO_MM);
    keyboardComponent.setKeyPressBaseOctave(5);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
    startTimer(500);
}

void MainComponent::initializeMidiMessagesBox() {
    addAndMakeVisible(midiMessagesBox);
    numMessagesInEditor = 0;
    midiMessagesBox.setMultiLine(true);
    midiMessagesBox.setReturnKeyStartsNewLine(true);
    midiMessagesBox.setReadOnly(true);
    midiMessagesBox.setScrollbarsShown(true);
    midiMessagesBox.setCaretVisible(false);
    midiMessagesBox.setPopupMenuEnabled(true);
    midiMessagesBox.setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 14, juce::Font::plain));
    std::string logo = R"(
.____       _____    _______            ________
|    |     /     \   \      \           \_____  \
|    |    /  \ /  \  /   |   \   ______   _(__  <
|    |___/    Y    \/    |    \ /_____/  /       \
|_______ \____|__  /\____|__  /         /______  /
        \/       \/         \/                 \/
)";
    midiMessagesBox.setText(logo);
}

void MainComponent::initializeMidiOutput() {
    addAndMakeVisible(midiOutputListLabel);
    midiOutputListLabel.setText("MIDI Output:", juce::dontSendNotification);
    midiOutputListLabel.attachToComponent(&midiOutputList, true);

    addAndMakeVisible(midiOutputList);
    midiOutputList.setTextWhenNoChoicesAvailable("No MIDI Outputs Enabled");
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();

    juce::StringArray midiOutputNames;

    for (auto output : midiOutputs) {
        midiOutputNames.add(output.name);
    }

    midiOutputList.addItemList(midiOutputNames, 1);
    midiOutputList.onChange = [this] {
        setMidiOutput(midiOutputList.getSelectedItemIndex());
    };

    setMidiOutput(0);

    // if no enabled devices were found just use the first one in the list
    if (midiOutputList.getSelectedId() == 0)
        setMidiOutput(0);
}

void MainComponent::initializeMidiInput() {
    addAndMakeVisible(midiInputListLabel);
    midiInputListLabel.setText("MIDI Input:", juce::dontSendNotification);
    midiInputListLabel.attachToComponent(&midiInputList, true);

    addAndMakeVisible(midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
    auto midiInputs = juce::MidiInput::getAvailableDevices();

    juce::StringArray midiInputNames;

    for (auto input : midiInputs)
        midiInputNames.add(input.name);

    midiInputList.addItemList(midiInputNames, 1);
    midiInputList.onChange = [this] {
        setMidiInput(midiInputList.getSelectedItemIndex());
    };

    // find the first enabled device and use that by default
    for (auto input : midiInputs) {
        if (deviceManager.isMidiInputDeviceEnabled(input.identifier)) {
            setMidiInput(midiInputs.indexOf(input));
            break;
        }
    }

    // if no enabled devices were found just use the first one in the list
    if (midiInputList.getSelectedId() == 0)
        setMidiInput(0);
}

void MainComponent::setMidiOutput(int index) {
    auto list = juce::MidiOutput::getAvailableDevices();
    auto newOutput = list[index];
    deviceManager.setDefaultMidiOutputDevice(newOutput.identifier);
    midiOutputList.setSelectedId(index + 1, juce::dontSendNotification);
    lastOutputIndex = index;
}

void MainComponent::setMidiInput(int index) {
    auto list = juce::MidiInput::getAvailableDevices();
    deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier,
                                                this);

    auto newInput = list[index];

    if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);

    deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
    midiInputList.setSelectedId(index + 1, juce::dontSendNotification);

    lastInputIndex = index;
}

void MainComponent::handleIncomingMidiMessage(
    juce::MidiInput *source, const juce::MidiMessage &message) {
    const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput,
                                                        true);
    // keyboardState.processNextMidiEvent(message);
    postMessageToList(message, source->getName());
}

void MainComponent::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    setRow0ButtonsBounds();
    setRow1ButtonsBounds();
    setRow2ButtonsBounds();
    setRow3ButtonsBounds();
    setRow4ButtonsBounds();
    setEncodersBounds();
    setJoystickBounds();
    setMidiKeyboardBounds();
    setMidiMessagesBoxBounds();
    midiOutputList.setBounds(LEFT_EDGE_PADDING * PX_TO_MM + 50, 5,
                             5 * KEY_WIDTH * PX_TO_MM,
                             VERTICAL_KEY_SPACING * PX_TO_MM);
    midiInputList.setBounds(midiOutputList.getBounds().getX() +
                                midiOutputList.getBounds().getWidth() + 100,
                            5, 5 * KEY_WIDTH * PX_TO_MM,
                            VERTICAL_KEY_SPACING * PX_TO_MM);
}

void MainComponent::setRow0ButtonsBounds() {
    double x =
        (LEFT_EDGE_PADDING + (5 * KEY_WIDTH) + (5 * HORIZONTAL_KEY_SPACING)) *
        PX_TO_MM;
    for (auto *button : row0) {
        button->setBounds(x, TOP_EDGE_PADDING * PX_TO_MM, KEY_WIDTH * PX_TO_MM,
                          KEY_HEIGHT * PX_TO_MM);
        x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    }
}

void MainComponent::setRow1ButtonsBounds() {
    double x =
        (LEFT_EDGE_PADDING + (5 * KEY_WIDTH) + (5 * HORIZONTAL_KEY_SPACING)) *
        PX_TO_MM;
    for (auto *button : row1) {
        button->setBounds(
            x,
            (TOP_EDGE_PADDING + KEY_HEIGHT + VERTICAL_KEY_SPACING) * PX_TO_MM,
            KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    }
}

void MainComponent::setRow2ButtonsBounds() {
    double x =
        (LEFT_EDGE_PADDING + (5 * KEY_WIDTH) + (5 * HORIZONTAL_KEY_SPACING)) *
        PX_TO_MM;
    for (auto *button : row2) {
        button->setBounds(
            x,
            (TOP_EDGE_PADDING + 2 * (KEY_HEIGHT + VERTICAL_KEY_SPACING)) *
                PX_TO_MM,
            KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    }
}

void MainComponent::setRow3ButtonsBounds() {
    bool threeKeyGroup = true;
    int keyNumberInGroup = 0;
    double x =
        (LEFT_EDGE_PADDING + (2 * KEY_WIDTH) + (2 * HORIZONTAL_KEY_SPACING) +
         ((KEY_WIDTH + HORIZONTAL_KEY_SPACING) / 2)) *
        PX_TO_MM;
    for (int i = 0; i < row3.size(); i++) {
        auto *button = row3[i];
        button->setBounds(
            x,
            (TOP_EDGE_PADDING + 3 * (KEY_HEIGHT + VERTICAL_KEY_SPACING)) *
                PX_TO_MM,
            KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        if ((threeKeyGroup && keyNumberInGroup == 2) ||
            (!threeKeyGroup && keyNumberInGroup == 1)) {
            x += 2 * (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
            threeKeyGroup = !threeKeyGroup;
            keyNumberInGroup = 0;
        } else {
            x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
            keyNumberInGroup++;
        }
    }
}

void MainComponent::setRow4ButtonsBounds() {
    double x =
        (LEFT_EDGE_PADDING + (2 * KEY_WIDTH) + (2 * HORIZONTAL_KEY_SPACING)) *
        PX_TO_MM;
    for (auto *button : row4) {
        button->setBounds(
            x,
            (TOP_EDGE_PADDING + 4 * (KEY_HEIGHT + VERTICAL_KEY_SPACING)) *
                PX_TO_MM,
            KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    }
}

void MainComponent::setJoystickBounds() {
    double x = LEFT_EDGE_PADDING * PX_TO_MM;
    double y =
        (TOP_EDGE_PADDING + 3 * (KEY_HEIGHT + VERTICAL_KEY_SPACING)) * PX_TO_MM;
    double width = ((2 * KEY_WIDTH) + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    double height = ((2 * KEY_HEIGHT) + VERTICAL_KEY_SPACING) * PX_TO_MM;
    joystick.setBounds(x, y, width, height);
}

void MainComponent::setEncodersBounds() {
    double x =
        (LEFT_EDGE_PADDING + (10 * KEY_WIDTH) + (10 * HORIZONTAL_KEY_SPACING) +
         ((KEY_WIDTH + HORIZONTAL_KEY_SPACING) / 2)) *
        PX_TO_MM;
    for (auto *encoder : encoders) {
        encoder->setBounds(x, TOP_EDGE_PADDING * PX_TO_MM, KEY_WIDTH * PX_TO_MM,
                           KEY_HEIGHT * PX_TO_MM);
        x += ((KEY_WIDTH + HORIZONTAL_KEY_SPACING) * 1.5) * PX_TO_MM;
    }
}

void MainComponent::setMidiKeyboardBounds() {
    double x =
        (LEFT_EDGE_PADDING + (2 * KEY_WIDTH) + (1.5 * HORIZONTAL_KEY_SPACING)) *
        PX_TO_MM;
    double y =
        (TOP_EDGE_PADDING + (3 * KEY_HEIGHT) + (2.5 * VERTICAL_KEY_SPACING)) *
        PX_TO_MM;
    double width =
        ((14 * KEY_WIDTH) + (14 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    double height = ((2 * KEY_HEIGHT) + (2 * VERTICAL_KEY_SPACING)) * PX_TO_MM;
    keyboardComponent.setBounds(x, y, width, height);
}

void MainComponent::setMidiMessagesBoxBounds() {
    double x = LEFT_EDGE_PADDING;
    double y = TOP_EDGE_PADDING * PX_TO_MM;
    double width = ((5 * KEY_WIDTH) + (5 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    double height = ((3 * KEY_HEIGHT) + (2 * VERTICAL_KEY_SPACING)) * PX_TO_MM;
    midiMessagesBox.setBounds(x, y, width, height);
}

void MainComponent::buttonClicked(juce::Button *button) {
//    auto name = button->getName();
//    int ccType = 0;
//    int ccValue = BUTTON_RELEASED;
//
//    // Buttons
//    if (name == "R0C5") {
//        ccType = LOOP_BUTTON;
//    } else if (name == "R0C6") {
//        ccType = LOOP_IN_BUTTON;
//    } else if (name == "R0C7") {
//        ccType = LOOP_OUT_BUTTON;
//    } else if (name == "R0C8") {
//        if (isCtrlDown) {
//            increaseOctave();
//            ccType = OCTAVE_CHANGE;
//            ccValue = getCCValueForCurrentOctave();
//        } else {
//            ccType = PLUS_BUTTON;
//        }
//    } else if (name == "R0C9") {
//        if (isCtrlDown) {
//            decreaseOctave();
//            ccType = OCTAVE_CHANGE;
//            ccValue = getCCValueForCurrentOctave();
//        } else {
//            ccType = MINUS_BUTTON;
//        }
//    } else if (name == "R1C5") {
//        ccType = CUT_BUTTON;
//    } else if (name == "R1C6") {
//        ccType = PASTE_BUTTON;
//    } else if (name == "R1C7") {
//        ccType = SLICE_BUTTON;
//    } else if (name == "R1C8") {
//        ccType = SAVE_BUTTON;
//    } else if (name == "R1C9") {
//        ccType = UNDO_BUTTON;
//    } else if (name == "R2C5") {
//        ccType = CONTROL_BUTTON;
//    } else if (name == "R2C6") {
//        ccType = RECORD_BUTTON;
//    } else if (name == "R2C7") {
//        ccType = PLAY_BUTTON;
//    } else if (name == "R2C8") {
//        ccType = STOP_BUTTON;
//    } else if (name == "R2C9") {
//        ccType = SETTINGS_BUTTON;
//    } else if (name == "R2C10") {
//        ccType = TEMPO_BUTTON;
//    } else if (name == "R2C11") {
//        ccType = MIXER_BUTTON;
//    } else if (name == "R2C12") {
//        ccType = TRACKS_BUTTON;
//    } else if (name == "R2C13") {
//        ccType = PLUGINS_BUTTON;
//    } else if (name == "R2C14") {
//        ccType = MODIFIERS_BUTTON;
//    } else if (name == "R2C15") {
//        ccType = SEQUENCERS_BUTTON;
//    }
//
//    sendCCMessage(MIDI_CHANNEL, ccType, ccValue);
}

void MainComponent::buttonStateChanged(juce::Button *button) {
    auto newButtonState = button->getState();
    auto previousButtonState = previousButtonStates.at(button->getName());
    // only fire on button push and button release
    auto buttonPressed = newButtonState == juce::Button::ButtonState::buttonDown &&
            previousButtonState == juce::Button::ButtonState::buttonOver;
    auto buttonReleased = previousButtonState == juce::Button::ButtonState::buttonDown;
    if (buttonPressed || buttonReleased) {
        auto name = button->getName();
        int ccType = 0;
        int ccValue = BUTTON_RELEASED;
        if (button->getState() == juce::Button::ButtonState::buttonDown)
            ccValue = BUTTON_PRESSED;

        // Buttons
        if (name == "R0C5") {
            ccType = LOOP_BUTTON;
        } else if (name == "R0C6") {
            ccType = LOOP_IN_BUTTON;
        } else if (name == "R0C7") {
            ccType = LOOP_OUT_BUTTON;
        } else if (name == "R0C8") {
            if (isCtrlDown) {
                // Only send this on button down
                if (button->getState() == juce::Button::ButtonState::buttonDown) {
                    increaseOctave();
                    ccType = OCTAVE_CHANGE;
                    ccValue = getCCValueForCurrentOctave();
                }
            } else {
                ccType = PLUS_BUTTON;
            }
        } else if (name == "R0C9") {
            if (isCtrlDown) {
                // Only send this on button down
                if (button->getState() == juce::Button::ButtonState::buttonDown) {
                    decreaseOctave();
                    ccType = OCTAVE_CHANGE;
                    ccValue = getCCValueForCurrentOctave();
                }
            } else {
                ccType = MINUS_BUTTON;
            }
        } else if (name == "R1C5") {
            ccType = CUT_BUTTON;
        } else if (name == "R1C6") {
            ccType = PASTE_BUTTON;
        } else if (name == "R1C7") {
            ccType = SLICE_BUTTON;
        } else if (name == "R1C8") {
            ccType = SAVE_BUTTON;
        } else if (name == "R1C9") {
            ccType = UNDO_BUTTON;
        } else if (name == "R2C5") {
            ccType = CONTROL_BUTTON;
        } else if (name == "R2C6") {
            ccType = RECORD_BUTTON;
        } else if (name == "R2C7") {
            ccType = PLAY_BUTTON;
        } else if (name == "R2C8") {
            ccType = STOP_BUTTON;
        } else if (name == "R2C9") {
            ccType = SETTINGS_BUTTON;
        } else if (name == "R2C10") {
            ccType = TEMPO_BUTTON;
        } else if (name == "R2C11") {
            ccType = MIXER_BUTTON;
        } else if (name == "R2C12") {
            ccType = TRACKS_BUTTON;
        } else if (name == "R2C13") {
            ccType = PLUGINS_BUTTON;
        } else if (name == "R2C14") {
            ccType = MODIFIERS_BUTTON;
        } else if (name == "R2C15") {
            ccType = SEQUENCERS_BUTTON;
        }

        sendCCMessage(MIDI_CHANNEL, ccType, ccValue);
    }

    // update the buttonState
    previousButtonStates.at(button->getName()) = newButtonState;
}

void MainComponent::encoderDoubleClicked(Encoder *encoder) {
    auto encoderName = encoder->getName();
    if (encoderName == "encoder1") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_1_BUTTON, BUTTON_RELEASED);
    } else if (encoderName == "encoder2") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_2_BUTTON, BUTTON_RELEASED);
    } else if (encoderName == "encoder3") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_3_BUTTON, BUTTON_RELEASED);
    } else if (encoderName == "encoder4") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_4_BUTTON, BUTTON_RELEASED);
    }
}

void MainComponent::encoderIncreased(Encoder *encoder) {
    auto encoderName = encoder->getName();
    if (encoderName == "encoder1") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_1, ENCODER_INCREASED);
    } else if (encoderName == "encoder2") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_2, ENCODER_INCREASED);
    } else if (encoderName == "encoder3") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_3, ENCODER_INCREASED);
    } else if (encoderName == "encoder4") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_4, ENCODER_INCREASED);
    }
}

void MainComponent::encoderDecreased(Encoder *encoder) {
    auto encoderName = encoder->getName();
    if (encoderName == "encoder1") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_1, ENCODER_DECREASED);
    } else if (encoderName == "encoder2") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_2, ENCODER_DECREASED);
    } else if (encoderName == "encoder3") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_3, ENCODER_DECREASED);
    } else if (encoderName == "encoder4") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_4, ENCODER_DECREASED);
    }
}

void MainComponent::sendCCMessage(int channel, int type, int value) {
    auto message = juce::MidiMessage::controllerEvent(channel, type, value);
    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 -
                         startTime);
    deviceManager.getDefaultMidiOutput()->sendMessageNow(message);
}

void MainComponent::handleNoteOn(juce::MidiKeyboardState *, int midiChannel,
                                 int midiNoteNumber, float velocity) {
    if (!isAddingFromMidiInput) {
        auto message =
            juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
        message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
        deviceManager.getDefaultMidiOutput()->sendMessageNow(message);
    }
}

void MainComponent::handleNoteOff(juce::MidiKeyboardState *, int midiChannel,
                                  int midiNoteNumber, float /*velocity*/) {
    if (!isAddingFromMidiInput) {
        auto message = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
        message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
        deviceManager.getDefaultMidiOutput()->sendMessageNow(message);
    }
}

juce::String
MainComponent::getMidiMessageDescription(const juce::MidiMessage &m) {
    if (m.isNoteOn())
        return "Note on " + juce::MidiMessage::getMidiNoteName(
                                m.getNoteNumber(), true, true, 3);
    if (m.isNoteOff())
        return "Note off " + juce::MidiMessage::getMidiNoteName(
                                 m.getNoteNumber(), true, true, 3);
    if (m.isProgramChange())
        return "Program change " + juce::String(m.getProgramChangeNumber());
    if (m.isPitchWheel())
        return "Pitch wheel " + juce::String(m.getPitchWheelValue());
    if (m.isAftertouch())
        return "After touch " +
               juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true,
                                                  3) +
               ": " + juce::String(m.getAfterTouchValue());
    if (m.isChannelPressure())
        return "Channel pressure " + juce::String(m.getChannelPressureValue());
    if (m.isAllNotesOff())
        return "All notes off";
    if (m.isAllSoundOff())
        return "All sound off";
    if (m.isMetaEvent())
        return "Meta event";

    if (m.isController()) {
        juce::String name =
            juce::MidiMessage::getControllerName(m.getControllerNumber());
        if (name.isEmpty())
            name = "[" + juce::String(m.getControllerNumber()) + "]";

        return "CC " + name + ": " + juce::String(m.getControllerValue());
    }

    return juce::String::toHexString(m.getRawData(), m.getRawDataSize());
}

void MainComponent::logMessage(const juce::String &m) {
    if (numMessagesInEditor > MIDI_MESSAGE_BOX_MESSAGE_LIMIT) {
        // For performance, reset the text of the editor after a good number of
        // messages have been displayed
        numMessagesInEditor = 0;
        midiMessagesBox.setText("");
    }
    midiMessagesBox.moveCaretToEnd();
    midiMessagesBox.insertTextAtCaret(m + juce::newLine);
    numMessagesInEditor++;
}

void MainComponent::postMessageToList(const juce::MidiMessage &message,
                                      const juce::String &source) {
    (new IncomingMessageCallback(this, message, source))->post();
}

void MainComponent::addMessageToList(const juce::MidiMessage &message,
                                     const juce::String & /*source*/) {
    auto time = message.getTimeStamp() - startTime;

    auto hours = ((int)(time / 3600.0)) % 24;
    auto minutes = ((int)(time / 60.0)) % 60;
    auto seconds = ((int)time) % 60;
    auto millis = ((int)(time * 1000.0)) % 1000;

    auto timecode = juce::String::formatted("%02d:%02d:%02d.%03d", hours,
                                            minutes, seconds, millis);

    auto description = getMidiMessageDescription(message);

    juce::String midiMessageString(timecode + "  -  " + description);
    logMessage(midiMessageString);
}

void MainComponent::modifierKeysChanged(const juce::ModifierKeys &modifiers) {
    if (modifiers.isCtrlDown()) {
        // Ctrl was pressed
        isCtrlDown = true;
        sendCCMessage(MIDI_CHANNEL, CONTROL_BUTTON, BUTTON_PRESSED);
    } else if (!modifiers.isCtrlDown() && isCtrlDown) {
        // Ctrl was released
        isCtrlDown = false;
        sendCCMessage(MIDI_CHANNEL, CONTROL_BUTTON, BUTTON_RELEASED);
    }
}

void MainComponent::sliderDragEnded(juce::Slider *slider) {
    if (slider == &joystick) {
        double midpoint = joystick.getRange().getLength() / 2;
        joystick.setValue(midpoint, juce::sendNotification);
    }
}

void MainComponent::sliderValueChanged(juce::Slider *slider) {
    if (slider == &joystick) {
        auto message =
            juce::MidiMessage::pitchWheel(MIDI_CHANNEL, joystick.getValue());
        message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 -
                             startTime);
        deviceManager.getDefaultMidiOutput()->sendMessageNow(message);
    }
}

int MainComponent::getCCValueForCurrentOctave() {
    // Cant send negative numbers as a CC value
    // -4 -> 0, -3 -> 1, ... 0 ->4, 4 -> 8
    int offset = (OCTAVE_MAX - OCTAVE_MIN) / 2;
    return currentOctave + offset;
}
void MainComponent::increaseOctave() {
    if (currentOctave < OCTAVE_MAX) {
        currentOctave++;
    }
}
void MainComponent::decreaseOctave() {
    if (currentOctave > OCTAVE_MIN) {
        currentOctave--;
    }
}

bool MainComponent::keyStateChanged(bool isKeyDown) {
    if (!isCtrlDown) {
        auto newPlusIsDown = juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress('=').getKeyCode());
        auto newMinusIsDown = juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress('-').getKeyCode());
        if (newPlusIsDown != isPlusDown) {
            isPlusDown = newPlusIsDown;
            if (newPlusIsDown)
                sendCCMessage(MIDI_CHANNEL, PLUS_BUTTON, BUTTON_PRESSED);
            else
                sendCCMessage(MIDI_CHANNEL, PLUS_BUTTON, BUTTON_RELEASED);
        }

        if (newMinusIsDown != isMinusDown) {
            isMinusDown = newMinusIsDown;
            if (newMinusIsDown)
                sendCCMessage(MIDI_CHANNEL, MINUS_BUTTON, BUTTON_PRESSED);
            else
                sendCCMessage(MIDI_CHANNEL, MINUS_BUTTON, BUTTON_RELEASED);
        }

    }

    // Pass the event up to whoever wants it
    return false;
}
