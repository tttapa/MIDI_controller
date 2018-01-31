#include "DigitalLatch.h"
#include "MIDI_Controller.h"

using namespace ExtIO;

DigitalLatch::DigitalLatch(pin_t pin, uint8_t msg, uint8_t note, uint8_t channel, uint8_t velocity, unsigned long latchTime) // Constructor
{
  ExtIO::pinMode(pin, INPUT_PULLUP); // Enable the internal pull-up resistor on the pin with the button/switch
  this->pin = pin;
  this->msg = msg;
  this->note = note;
  this->channel = channel;
  this->velocity = velocity;
  this->latchTime = latchTime;
}

DigitalLatch::~DigitalLatch() // Destructor
{
  ExtIO::pinMode(pin, INPUT); // make it a normal input again, without the internal pullup resistor.
}

void DigitalLatch::push() //
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
	}
}

void DigitalLatch::release() //
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
	}
}

void DigitalLatch::invert() // Invert the button state (send Note On event when released, Note Off when pressed)
{
  invertState = true;
}

void DigitalLatch::refresh() // Check if the button state changed, if so, send a MIDI Note On, after a non-blocking delay of "latchTime", send a Note Off
{
  bool state = ExtIO::digitalRead(pin) ^ invertState; // read the button state and invert it if "invert" is true
  
  state = digitalMap(pin, state);
  
  if (state != oldState)         // If the switch changed position
  {
    if (noteOffSent) // If the note is turned off
    {
      this->push();
      noteOnTime = millis();                                                                    // store the time of the note on message
      noteOffSent = false;                                                                      // The note is turned on
    }
    else // If the button is switched again, before latch time is reached
    {
      this->release();	// Turn off the note
	  this->push();		// Immediately turn the note on again
	  noteOnTime = millis();                                                                     // store the time of the note on message
    }
    oldState = state;
  }
  if (millis() - noteOnTime > latchTime && !noteOffSent) // if the time elapsed since the Note On event is greater than the latch time, and if the note is still on
  {
    this->release();
    noteOffSent = true;                                                                        // The note is turned off
  }
}

void DigitalLatch::map(int (*fn)(int, int)) // change the function pointer for digitalMap to a new function. It will be applied to the raw digital input value in Digital::refresh()
{
  digitalMap = fn;
}