#include "Digital.h"
#include "MIDI_Controller.h"

using namespace ExtIO;

Digital::Digital(pin_t pin, uint8_t msg, uint8_t note, uint8_t channel, uint8_t velocity) // Constructor
{
  ExtIO::pinMode(pin, INPUT_PULLUP); // Enable the internal pull-up resistor on the pin with the button/switch
  this->pin = pin;
  this->msg = msg;
  this->note = note;
  this->channel = channel;
  this->velocity = velocity;
}

Digital::~Digital() // Destructor
{
  ExtIO::pinMode(pin, INPUT); // make it a normal input again, without the internal pullup resistor.
}

void Digital::push() //
{
	switch (msg) {
		case NOTE_ON:
			MIDI_Controller.MIDI()->send(NOTE_ON, channel + channelOffset * channelsPerBank, note + addressOffset * channelsPerBank, velocity);
			break;
		case CONTROL_CHANGE:
			MIDI_Controller.MIDI()->send(CONTROL_CHANGE, channel + channelOffset * channelsPerBank, note + addressOffset * channelsPerBank, 127);
			break;
		case PROGRAM_CHANGE:
			if (!invertState)
				MIDI_Controller.MIDI()->send(PROGRAM_CHANGE, channel + channelOffset * channelsPerBank, note + addressOffset * channelsPerBank);
			break;
		case PITCH_BEND:
			MIDI_Controller.MIDI()->send(PITCH_BEND, channel + channelOffset * channelsPerBank, 127, note + addressOffset * channelsPerBank);
			break;
	}
}

void Digital::release() //
{
	switch (msg) {
		case NOTE_ON:
			MIDI_Controller.MIDI()->send(NOTE_OFF, channel + channelOffset * channelsPerBank, note + addressOffset * channelsPerBank, velocity);
			break;
		case CONTROL_CHANGE:
			MIDI_Controller.MIDI()->send(CONTROL_CHANGE, channel + channelOffset * channelsPerBank, note + addressOffset * channelsPerBank, 0);
			break;
		case PROGRAM_CHANGE:
			if (invertState)
				MIDI_Controller.MIDI()->send(PROGRAM_CHANGE, channel + channelOffset * channelsPerBank, note + addressOffset * channelsPerBank);
			break;
		case PITCH_BEND:
			MIDI_Controller.MIDI()->send(PITCH_BEND, channel + channelOffset * channelsPerBank, 0, note + addressOffset * channelsPerBank);
			break;
	}
}

void Digital::invert() // Invert the button state (send Note On event when released, Note Off when pressed)
{
  invertState = true;
}

void Digital::refresh() // Check if the button state changed, and send a MIDI Note On or Off accordingly
{
  bool state = ExtIO::digitalRead(pin) ^ invertState; // read the button state and invert it if "invert" is true

  if (millis() - prevBounceTime > debounceTime)
  {
    int8_t stateChange = digitalMap(pin, state) - buttonState;

    if (stateChange == falling)
    { // Button is pushed
      buttonState = state;
	  this->push();
    }

    if (stateChange == rising)
    { // Button is released
      buttonState = state;
	  this->release();
    }
  }
  if (state != prevState)
  {
    prevBounceTime = millis();
    prevState = state;
  }
}

void Digital::map(int (*fn)(int, int)) // change the function pointer for digitalMap to a new function. It will be applied to the raw digital input value in Digital::refresh()
{
  digitalMap = fn;
}