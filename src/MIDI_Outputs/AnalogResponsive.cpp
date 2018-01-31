#include "Arduino.h"
#include "AnalogResponsive.h"
#include "MIDI_Controller.h"

using namespace ExtIO;

AnalogResponsive::AnalogResponsive(pin_t analogPin, uint8_t controllerNumber, uint8_t channel) // Constructor
{
  this->analogPin = analogPin;
  this->controllerNumber = controllerNumber;
  this->channel = channel;
  this->respAnalog = new ResponsiveAnalogRead(analogPin, true);
}

AnalogResponsive::~AnalogResponsive() // Destructor
{
  delete respAnalog;
}

void AnalogResponsive::push() //
{
	MIDI_Controller.MIDI()->send(CC, channel + channelOffset * channelsPerBank, controllerNumber + addressOffset * channelsPerBank, 1023); // send a Control Change MIDI event
}

void AnalogResponsive::release() //
{
	MIDI_Controller.MIDI()->send(CC, channel + channelOffset * channelsPerBank, controllerNumber + addressOffset * channelsPerBank, 0); // send a Control Change MIDI event
}

void AnalogResponsive::invert() // Invert the button state (send Note On event when released, Note Off when pressed)
{
  invertState = true;
}

void AnalogResponsive::refresh() // read the analog value, update the average, map it to a MIDI value, check if it changed since last time, if so, send Control Change message over MIDI
{
  unsigned int input = ExtIO::analogRead(analogPin); // read the raw analog input value
  if (invertState) input = 1023-input;				 // invert the scale
  uint16_t value = analogMap(analogPin, input); 	 // apply the analogMap function to the value (identity function f(x) = x by default)
  respAnalog->update(value); 						 // update the responsive analog average
  if (respAnalog->hasChanged()) 				 	 // if the value changed since last time
  {
    value = respAnalog->getValue(); // get the responsive analog average value
	MIDI_Controller.MIDI()->send(CC, channel + channelOffset * channelsPerBank, controllerNumber + addressOffset * channelsPerBank, value); // send a Control Change MIDI event
  }
}

void AnalogResponsive::map(int (*fn)(int, int)) // change the function pointer for analogMap to a new function. It will be applied to the raw analog input value in Analog::refresh()
{
  analogMap = fn;
}