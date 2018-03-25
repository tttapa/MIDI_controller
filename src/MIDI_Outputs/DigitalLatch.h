#ifndef DIGLAT_h_
#define DIGLAT_h_

#include "Arduino.h"
#include "MIDI_Control_Element.h"
#include "../ExtendedInputOutput/ExtendedInputOutput.h"

class DigitalLatch : public MIDI_Control_Element
{
public:
  DigitalLatch(pin_t pin, uint8_t msg, uint8_t note, uint8_t channel, uint8_t velocity = 127, unsigned long latchTime = 100); // Constructor
  ~DigitalLatch();                                                                                     // Destructor
  void push();
  void release();
  void invert();                                                         // Invert the button state (send Note On event when released, Note Off when pressed)
  void map(int (*fn)(int, int));                                       	 // Change the function pointer for digitalMap to a new function. It will be applied to the raw digital input value in DigitalLatch::refresh()

  private:
  void refresh(); // Check if the button state changed, if so, send a MIDI Note On, after a non-blocking delay of "latchTime", send a Note Off
  int (*digitalMap)(int, int) = identity; // function pointer to identity function f(x) → x

  static int identity(int p, int x)
  { // identity function f(x) → x
    return x;
  }
  
  pin_t pin;
  uint8_t msg, note, channel, velocity;
  bool oldState = HIGH;
  bool noteOffSent = true;
  unsigned long latchTime;
  unsigned long noteOnTime;
  
  bool invertState = false;
};

#endif
