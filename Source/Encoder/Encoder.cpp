#include "Encoder.h"

Encoder::Encoder() {
    setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    setRange(0.0, 127.0, 1.0);
    setValue(0.0);
    setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRotaryParameters(0, juce::MathConstants<double>::twoPi, false);
}

Encoder::~Encoder() {
}

void Encoder::addListener(Encoder::Listener *l) {
    listeners.add(l);
}

void Encoder::removeListener(Encoder::Listener *l) {
    listeners.remove(l);
}

void Encoder::mouseDoubleClick(const juce::MouseEvent&) {
    listeners.call([this](Listener &l) { l.encoderDoubleClicked(this); });
}

void Encoder::valueChanged() {
    // Edge cases whenever we roll over from the larger end of values to the smaller end
    // or vice versa.
    // Also the difference between values is not guaranteed to be equal to interval
    // Depending on mouse speed and other factors, it is equal to 
    // (minimum + (interval * an integer)).
    // Therefore, we simply check to see if the change between previous and current value
    // is greater than half of the entire range
    double range = getRange().getEnd() - getRange().getStart();
    if (std::abs(getValue() - previousValue) > (range / 2.0)) {
        if (getValue() > previousValue) {
                listeners.call([this](Listener &l) { l.encoderDecreased(this); });
        } else {
                listeners.call([this](Listener &l) { l.encoderIncreased(this); });
        }
    } else {
        if (getValue() > previousValue) {
                listeners.call([this](Listener &l) { l.encoderIncreased(this); });
        } else {
                listeners.call([this](Listener &l) { l.encoderDecreased(this); });
        }
    }
    previousValue = getValue();
}