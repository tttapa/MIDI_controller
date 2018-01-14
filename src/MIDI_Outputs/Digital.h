#ifndef DIG_h_
#define DIG_h_

#include "Arduino.h"
#include "MIDI_Control_Element.h"
#include "../Settings/Settings.h"
#include "../ExtendedInputOutput/ExtendedInputOutput.h"

class Digital : public MIDI_Control_Element
{
public:
  Digital(pin_t pin, uint8_t note, uint8_t channel, uint8_t velocity = 127); // Constructor
  ~Digital();                                                                // Destructor
  void invert();                                                             // Invert the button state (send Note On event when released, Note Off when pressed)
  void map(int (*fn)(int));                                           		 // Change the function pointer for digitalMap to a new function. It will be applied to the raw digital input value in Digital::refresh()

private:
  void refresh(); // Check if the button state changed, and send a MIDI Note On or Off accordingly
  int (*digitalMap)(int) = identity; // function pointer to identity function f(x) → x

  static int identity(int x)
  { // identity function f(x) → x
    return x;
  }
  
  pin_t pin;
  uint8_t note, channel, velocity;
  bool prevState = HIGH, buttonState = HIGH;
  unsigned long prevBounceTime = 0;

  bool invertState = false;

  const static unsigned long debounceTime = BUTTON_DEBOUNCE_TIME;

  const static int8_t falling = LOW - HIGH;
  const static int8_t rising = HIGH - LOW;
};

#endif
