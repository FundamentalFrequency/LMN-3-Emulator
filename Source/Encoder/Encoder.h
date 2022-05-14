#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class Encoder : public juce::Slider {
  public:
    Encoder();
    ~Encoder() override;
    class Listener {
      public:
        virtual ~Listener() = default;
        virtual void encoderDoubleClicked(Encoder * /*encoder*/) {}
        virtual void encoderIncreased(Encoder * /*encoder*/) {}
        virtual void encoderDecreased(Encoder * /*encoder*/) {}
    };

    void addListener(Listener *l);
    void removeListener(Listener *l);

    void mouseDoubleClick(const juce::MouseEvent &) override;
    void valueChanged() override;
    void modifierKeysChanged(const juce::ModifierKeys &) override;

  private:
    juce::ListenerList<Listener> listeners;
    double previousValue = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Encoder)
};