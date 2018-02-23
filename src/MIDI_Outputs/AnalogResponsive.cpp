#include "Arduino.h"
#include <AnalogResponsive.h>
#include "MIDI_Controller.h"

using namespace ExtIO;

AnalogResponsive::AnalogResponsive(pin_t analogPin, uint8_t msg, uint8_t controllerNumber, uint8_t channel) // Constructor
{
  this->analogPin = analogPin;
  this->msg = msg;
  this->controllerNumber = controllerNumber;
  this->channel = channel;
  this->respAnalog = new ResponsiveAnalogRead(analogPin, true);
}

AnalogResponsive::~AnalogResponsive() // Destructor
{
  delete respAnalog;
}

void AnalogResponsive::push(uint16_t value) //
{
	switch (msg) {
		case NOTE_ON:
			MIDI_Controller.MIDI()->send(NOTE_ON, channel + channelOffset * channelsPerBank, value, 127);
			break;
		case CONTROL_CHANGE:
			MIDI_Controller.MIDI()->send(CONTROL_CHANGE, channel + channelOffset * channelsPerBank, controllerNumber + addressOffset * channelsPerBank, value);
			break;
		case PROGRAM_CHANGE:
			MIDI_Controller.MIDI()->send(PROGRAM_CHANGE, channel + channelOffset * channelsPerBank, value);
			break;
		case PITCH_BEND:
			MIDI_Controller.MIDI()->send(PITCH_BEND, channel + channelOffset * channelsPerBank, value, value >> 7);
			break; 
	}
}

void AnalogResponsive::release(uint16_t value) //
{
	this->push(value);
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
	if ( msg == PITCH_BEND)
	{
		value = value << 4; // make it a 14-bit number (pad with 4 zeros)		
	}
	this->push(value);				// send a MIDI event
  }
}

void AnalogResponsive::map(int (*fn)(int, int)) // change the function pointer for analogMap to a new function. It will be applied to the raw analog input value in Analog::refresh()
{
  analogMap = fn;
}
