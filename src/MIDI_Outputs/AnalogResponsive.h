#ifndef AnalogResponsive_h_
#define AnalogResponsive_h_

#include "Arduino.h"
#include "../Settings/Settings.h"
#include "MIDI_Control_Element.h"
#include "../ExtendedInputOutput/ExtendedInputOutput.h"
#include <ResponsiveAnalogRead.h>

class AnalogResponsive : public MIDI_Control_Element
{
public:
  AnalogResponsive(pin_t analogPin, uint8_t controllerNumber, uint8_t channel); // Constructor
  ~AnalogResponsive();                                                          // Destructor
  void push();
  void release();
  void invert();                                                      	// Invert the analog scale
  void map(int (*fn)(int, int));                                        // Change the function pointer for analogMap to a new function. It will be applied to the raw analog input value in Analog::refresh()

private:
  void refresh(); // Read the analog input value, update the average, map it to a MIDI value, check if it changed since last time, if so, send Control Change message over MIDI

  ResponsiveAnalogRead *respAnalog;
  pin_t analogPin;
  uint8_t controllerNumber, channel, oldVal = -1;
  int (*analogMap)(int, int) = identity; // function pointer to identity function f(x) → x

  static int identity(int p, int x)
  { // identity function f(x) → x
    return x;
  }
  
  bool invertState = false;
};

#endif // AnalogResponsive_h_
