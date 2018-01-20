#ifndef DIGITALCC_H_
#define DIGITALCC_H_

#include "Arduino.h"
#include "MIDI_Control_Element.h"
#include "../Settings/Settings.h"
#include "../ExtendedInputOutput/ExtendedInputOutput.h"

class DigitalCC : public MIDI_Control_Element
{
public:
  DigitalCC(pin_t pin, uint8_t controller, uint8_t channel);  // Constructor
  ~DigitalCC();                                               // Destructor
  void invert();                                              // Invert the button state
  void map(int (*fn)(int, int));                                   // Change the function pointer for digitalMap to a new function. It will be applied to the raw digital input value in Digital::refresh()

private:
  void refresh(); // Check if the button state changed, and send a MIDI CC accordingly
  int (*digitalMap)(int, int) = identity; // function pointer to identity function f(x) → x

  static int identity(int p, int x)
  { // identity function f(x) → x
    return x;
  }
  
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