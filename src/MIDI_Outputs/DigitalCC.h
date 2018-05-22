#ifndef DIGITALCC_H_
#define DIGITALCC_H_

#include "./MIDI_Control_Element.h"
#include "../Settings/Settings.h"
#include "../ExtendedInputOutput/ExtendedInputOutput.h"

class DigitalCC : public MIDI_Control_Element
{
public:
  DigitalCC(pin_t pin, uint8_t controller, uint8_t channel);  // Constructor
  ~DigitalCC();                                               // Destructor
  void invert();                                              // Invert the button state

private:
  void refresh(); // Check if the button state changed, and send a MIDI CC accordingly

  pin_t pin;
  uint8_t controller, channel;
  bool prevState = HIGH, buttonState = HIGH;
  unsigned long prevBounceTime = 0;

  bool invertState = false;

  const static unsigned long debounceTime = BUTTON_DEBOUNCE_TIME;

  const static int8_t falling = LOW - HIGH;
  const static int8_t rising = HIGH - LOW;
};

#endif