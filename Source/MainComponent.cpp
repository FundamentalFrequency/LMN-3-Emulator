#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(): 
    defaultMIDIOutput(juce::MidiOutput::openDevice(juce::MidiOutput::getDefaultDevice().identifier)),
    startTime(juce::Time::getMillisecondCounterHiRes() * 0.001)
{
    DBG("Initialized with default Midi Device: " + defaultMIDIOutput->getDeviceInfo().name);
    addAndMakeVisible(testButton);
    testButton.setButtonText("Test");
    testButton.addListener(this);
    setSize(DEVICE_WIDTH * PX_TO_MM, DEVICE_HEIGHT * PX_TO_MM);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setFont(juce::Font (16.0f));
    g.setColour(juce::Colours::white);
    g.drawText("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    testButton.setBounds(LEFT_EDGE_PADDING * PX_TO_MM, TOP_EDGE_PADDING * PX_TO_MM, KEY_WIDTH * PX_TO_MM, KEY_HEIGHT * PX_TO_MM);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &testButton) {
        auto message = juce::MidiMessage::controllerEvent(1, 1, 127);
        message.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 - startTime);
        defaultMIDIOutput->sendMessageNow(message);
    }
}