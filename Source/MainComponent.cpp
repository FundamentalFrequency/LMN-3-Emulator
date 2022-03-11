#include "MainComponent.h"
#include "MidiMappings.h"

MainComponent::MainComponent(): 
    defaultMIDIOutput(juce::MidiOutput::openDevice(juce::MidiOutput::getDefaultDevice().identifier)),
    startTime(juce::Time::getMillisecondCounterHiRes() * 0.001),
    keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard) {
    DBG("Initialized with default Midi Device: " + defaultMIDIOutput->getDeviceInfo().name);

    initializeRow0Buttons();
    initializeRow1Buttons();
    initializeRow2Buttons();
    initializeRow3Buttons();
    initializeRow4Buttons();

    initializeEncoders();

    joystick.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    joystick.setRange(-127, 127, .1);
    joystick.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(joystick);

    initializeKeyboard();

    initializeMidiMessagesBox();
    initializeMidiOutput();

    setSize(DEVICE_WIDTH * PX_TO_MM, DEVICE_HEIGHT * PX_TO_MM);
}

MainComponent::~MainComponent() {
    removeListenersFromRow(&row0);
    removeListenersFromRow(&row1);
    removeListenersFromRow(&row2);

    for (auto* encoder : encoders) {
        encoder->removeListener(this);
    }

    keyboardState.removeListener(this);
}

void MainComponent::timerCallback() {
    stopTimer();
    keyboardComponent.grabKeyboardFocus();
}

void MainComponent::removeListenersFromRow(juce::OwnedArray<juce::TextButton>* row) {
    for (auto* button : *row){
        button->removeListener(this);
    }
}

void MainComponent::initializeRow0Buttons() {
     for (int i = 5; i <= 9; i++) {
        auto* button = new juce::TextButton();
        row0.add(button);
        button->addListener(this);
        button->setName("R0C" + std::to_string(i));
        button->setButtonText("R0C" + std::to_string(i));
        addAndMakeVisible(button);
    }
}

void MainComponent::initializeRow1Buttons() {
     for (int i = 5; i <= 9; i++) {
        auto* button = new juce::TextButton();
        row1.add(button);
        button->addListener(this);
        button->setName("R1C" + std::to_string(i));
        button->setButtonText("R1C" + std::to_string(i));
        addAndMakeVisible(button);
    }
}

void MainComponent::initializeRow2Buttons() {
     for (int i = 5; i <= 15; i++) {
        auto* button = new juce::TextButton();
        row2.add(button);
        button->addListener(this);
        button->setName("R2C" + std::to_string(i));
        button->setButtonText("R2C" + std::to_string(i));
        addAndMakeVisible(button);
    }
}

void MainComponent::initializeRow3Buttons() {
     for (int i = 3; i <= 15; i++) {
        if (i != 6 && i != 9 && i != 13) {
            auto* button = new juce::TextButton();
            row3.add(button);
            button->addListener(this);
            button->setName("R3C" + std::to_string(i));
            button->setButtonText("R3C" + std::to_string(i));
            addAndMakeVisible(button);
        }
    }
}

void MainComponent::initializeRow4Buttons() {
     for (int i = 2; i <= 15; i++) {
        auto* button = new juce::TextButton();
        row4.add(button);
        button->addListener(this);
        button->setName("R4C" + std::to_string(i));
        button->setButtonText("R4C" + std::to_string(i));
        addAndMakeVisible(button);
    }
}

void MainComponent::initializeEncoders() {
    for (int i = 1; i <= 4; i++) {
        auto* encoder = new Encoder();
        encoders.add(encoder);
        encoder->setName("encoder" + std::to_string(i));
        encoder->addListener(this);
        addAndMakeVisible(encoder);
    }
}

void MainComponent::initializeKeyboard() {
    keyboardComponent.setWantsKeyboardFocus(true);
    keyboardComponent.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::shadowColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    keyboardComponent.setMidiChannel(MIDI_CHANNEL);
    keyboardComponent.setLowestVisibleKey(53);
    keyboardComponent.setAvailableRange(53, 76);
    keyboardComponent.setBlackNoteLengthProportion(.5);
    keyboardComponent.setBlackNoteWidthProportion(1.0);
    keyboardComponent.setKeyWidth((KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM);
    keyboardComponent.setKeyPressBaseOctave(5);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
    startTimer(500);
}

void MainComponent::initializeMidiMessagesBox() {
    addAndMakeVisible(midiMessagesBox);
    midiMessagesBox.setMultiLine(true);
    midiMessagesBox.setReturnKeyStartsNewLine(true);
    midiMessagesBox.setReadOnly(true);
    midiMessagesBox.setScrollbarsShown(true);
    midiMessagesBox.setCaretVisible(false);
    midiMessagesBox.setPopupMenuEnabled(true);
    midiMessagesBox.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    midiMessagesBox.setColour(juce::TextEditor::outlineColourId, juce::Colours::black);
    midiMessagesBox.setColour(juce::TextEditor::shadowColourId, juce::Colours::black);
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
    midiOutputList.onChange = [this] { setMidiOutput(midiOutputList.getSelectedItemIndex()); };

    setMidiOutput(0);

    // if no enabled devices were found just use the first one in the list
    if (midiOutputList.getSelectedId() == 0)
        setMidiOutput(0);
}

void MainComponent::setMidiOutput(int index)
{
    auto list = juce::MidiOutput::getAvailableDevices();
    auto newOuput = list[index];
    deviceManager.setDefaultMidiOutputDevice(newOuput.identifier);
    lastOutputIndex = index;
}

void MainComponent::paint(juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
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
    midiOutputList.setBounds(LEFT_EDGE_PADDING * PX_TO_MM + 50, 5, 5 * KEY_WIDTH * PX_TO_MM, VERTICAL_KEY_SPACING * PX_TO_MM);
}

void MainComponent::setRow0ButtonsBounds() {
    double x = (LEFT_EDGE_PADDING + (5 * KEY_WIDTH) + (5 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    for (auto* button : row0){
        button->setBounds(x, TOP_EDGE_PADDING * PX_TO_MM, KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    }
}

void MainComponent::setRow1ButtonsBounds() {
    double x = (LEFT_EDGE_PADDING + (5 * KEY_WIDTH) + (5 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    for (auto* button : row1){
        button->setBounds(x, (TOP_EDGE_PADDING + KEY_HEIGHT + VERTICAL_KEY_SPACING)  * PX_TO_MM, KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    }
}

void MainComponent::setRow2ButtonsBounds() {
    double x = (LEFT_EDGE_PADDING + (5 * KEY_WIDTH) + (5 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    for (auto* button : row2){
        button->setBounds(x, (TOP_EDGE_PADDING + 2 * (KEY_HEIGHT + VERTICAL_KEY_SPACING))  * PX_TO_MM, KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    }
}

void MainComponent::setRow3ButtonsBounds() {
    bool threeKeyGroup = true;
    int keyNumberInGroup = 0;
    double x = (LEFT_EDGE_PADDING + (2 * KEY_WIDTH) + (2 * HORIZONTAL_KEY_SPACING) + ((KEY_WIDTH + HORIZONTAL_KEY_SPACING) / 2)) * PX_TO_MM;
    for (int i = 0; i < row3.size(); i++) {
        auto* button = row3[i];
        button->setBounds(x, (TOP_EDGE_PADDING + 3 * (KEY_HEIGHT + VERTICAL_KEY_SPACING))  * PX_TO_MM, KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        if ((threeKeyGroup && keyNumberInGroup == 2) || (!threeKeyGroup && keyNumberInGroup == 1)) {
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
    double x = (LEFT_EDGE_PADDING + (2 * KEY_WIDTH) + (2 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    for (auto* button : row4){
        button->setBounds(x, (TOP_EDGE_PADDING + 4 * (KEY_HEIGHT + VERTICAL_KEY_SPACING))  * PX_TO_MM, KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        x += (KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    }
}

void MainComponent::setJoystickBounds() {
    double x = LEFT_EDGE_PADDING * PX_TO_MM;
    double y = (TOP_EDGE_PADDING + 3 * (KEY_HEIGHT + VERTICAL_KEY_SPACING))  * PX_TO_MM;
    double width = ((2 * KEY_WIDTH) + HORIZONTAL_KEY_SPACING) * PX_TO_MM;
    double height = ((2 * KEY_HEIGHT) + VERTICAL_KEY_SPACING) * PX_TO_MM;
    joystick.setBounds(x, y, width, height);
}

void MainComponent::setEncodersBounds() {
    double x = (LEFT_EDGE_PADDING + (10 * KEY_WIDTH) + (10 * HORIZONTAL_KEY_SPACING) + ((KEY_WIDTH + HORIZONTAL_KEY_SPACING) / 2)) * PX_TO_MM;
    for (auto* encoder : encoders){
        encoder->setBounds(x, TOP_EDGE_PADDING * PX_TO_MM, KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
        x += ((KEY_WIDTH + HORIZONTAL_KEY_SPACING) * 1.5)  * PX_TO_MM;
    }
}

void MainComponent::setMidiKeyboardBounds() {
    double x = (LEFT_EDGE_PADDING + (2 * KEY_WIDTH) + (1.5 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    double y = (TOP_EDGE_PADDING + (3 * KEY_HEIGHT) + (2.5 * VERTICAL_KEY_SPACING))  * PX_TO_MM;
    double width = ((14 * KEY_WIDTH) + (14 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    double height = ((2 * KEY_HEIGHT) + (2 *VERTICAL_KEY_SPACING)) * PX_TO_MM;
    keyboardComponent.setBounds(x, y, width, height);
}

void MainComponent::setMidiMessagesBoxBounds() {
    double x = LEFT_EDGE_PADDING * PX_TO_MM;
    double y = TOP_EDGE_PADDING * PX_TO_MM;
    double width = ((5 * KEY_WIDTH) + (4 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    double height = ((3 * KEY_HEIGHT) + (2 *VERTICAL_KEY_SPACING)) * PX_TO_MM;
    midiMessagesBox.setBounds(x, y, width, height);
}

void MainComponent::buttonClicked(juce::Button* button) {
    auto name = button->getName();
    int ccType = 0;
    int ccValue = BUTTON_RELEASED;

    // Buttons
    if (name == "R0C5") {
        ccType = LOOP_BUTTON;
    } else if (name == "R0C6") {
        ccType = LOOP_IN_BUTTON;
    } else if (name == "R0C7") {
        ccType = LOOP_OUT_BUTTON;
    } else if (name == "R0C8") {
        ccType = LEFT_BUTTON;
    } else if (name == "R0C9") {
        ccType = RIGHT_BUTTON;
    } else if (name == "R1C5") {
        ccType = CUT_BUTTON;
    } else if (name == "R1C6") {
        ccType = PASTE_BUTTON;
    } else if (name == "R1C7") {
        ccType = SLICE_BUTTON;
    } else if (name == "R1C8") {
        ;
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

void MainComponent::encoderDoubleClicked(Encoder* encoder) {
    auto encoderName = encoder->getName();
    if (encoderName == "encoder1") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_1_BUTTON, BUTTON_RELEASED);
    } else if (encoderName == "encoder2") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_2, BUTTON_RELEASED);
    } else if (encoderName == "encoder3") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_3, BUTTON_RELEASED);
    } else if (encoderName == "encoder4") {
        sendCCMessage(MIDI_CHANNEL, ENCODER_4, BUTTON_RELEASED);
    }
}

void MainComponent::encoderIncreased(Encoder* encoder) {
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

void MainComponent::encoderDecreased(Encoder* encoder) {
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
    message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
    deviceManager.getDefaultMidiOutput()->sendMessageNow(message);
}

void MainComponent::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001); 
}
 
void MainComponent::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) {
    auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber);
    m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
}

juce::String MainComponent::getMidiMessageDescription(const juce::MidiMessage& m) {
    if (m.isNoteOn())           return "Note on "          + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isNoteOff())          return "Note off "         + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isProgramChange())    return "Program change "   + juce::String (m.getProgramChangeNumber());
    if (m.isPitchWheel())       return "Pitch wheel "      + juce::String (m.getPitchWheelValue());
    if (m.isAftertouch())       return "After touch "      + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) +  ": " + juce::String(m.getAfterTouchValue());
    if (m.isChannelPressure())  return "Channel pressure " + juce::String (m.getChannelPressureValue());
    if (m.isAllNotesOff())      return "All notes off";
    if (m.isAllSoundOff())      return "All sound off";
    if (m.isMetaEvent())        return "Meta event";

    if (m.isController())
    {
        juce::String name = juce::MidiMessage::getControllerName(m.getControllerNumber());
        if (name.isEmpty())
            name = "[" + juce::String (m.getControllerNumber()) + "]";

        return "Controller " + name + ": " + juce::String(m.getControllerValue());
    }

    return juce::String::toHexString(m.getRawData(), m.getRawDataSize());
}

void MainComponent::logMessage(const juce::String& m) {
    midiMessagesBox.moveCaretToEnd();
    midiMessagesBox.insertTextAtCaret(m + juce::newLine);
}

void MainComponent::addMessageToList(const juce::MidiMessage& message, const juce::String& source) {
    auto time = message.getTimeStamp() - startTime;

    auto hours   = ((int) (time / 3600.0)) % 24;
    auto minutes = ((int) (time / 60.0)) % 60;
    auto seconds = ((int) time) % 60;
    auto millis  = ((int) (time * 1000.0)) % 1000;

    auto timecode = juce::String::formatted("%02d:%02d:%02d.%03d",
                                                hours,
                                                minutes,
                                                seconds,
                                                millis);

    auto description = getMidiMessageDescription(message);

    juce::String midiMessageString(timecode + "  -  " + description + " (" + source + ")"); // [7]
    logMessage(midiMessageString);
}



