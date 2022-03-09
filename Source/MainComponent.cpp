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

    keyboardComponent.setMidiChannel(MIDI_CHANNEL);
    keyboardComponent.setLowestVisibleKey(53);
    keyboardComponent.setAvailableRange(53, 76);
    keyboardComponent.setBlackNoteLengthProportion(.5);
    keyboardComponent.setBlackNoteWidthProportion(1.0);
    keyboardComponent.setKeyWidth((KEY_WIDTH + HORIZONTAL_KEY_SPACING) * PX_TO_MM);
    keyboardComponent.setKeyPressBaseOctave(5);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);

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
    double x = (LEFT_EDGE_PADDING + (2 * KEY_WIDTH) + (2 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    double y = (TOP_EDGE_PADDING + 3 * (KEY_HEIGHT + VERTICAL_KEY_SPACING))  * PX_TO_MM;
    double width = ((14 * KEY_WIDTH) + (14 * HORIZONTAL_KEY_SPACING)) * PX_TO_MM;
    double height = ((2 * KEY_HEIGHT) + VERTICAL_KEY_SPACING) * PX_TO_MM;
    keyboardComponent.setBounds(x, y, width, height);
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
    defaultMIDIOutput->sendMessageNow(message);
}

void MainComponent::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    auto m = juce::MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001); 
}
 
void MainComponent::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) {
    auto m = juce::MidiMessage::noteOff (midiChannel, midiNoteNumber);
    m.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
}


