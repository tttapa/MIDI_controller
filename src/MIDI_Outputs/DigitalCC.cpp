#include "DigitalCC.h"
#include "MIDI_Controller.h"

using namespace ExtIO;

DigitalCC::DigitalCC(pin_t pin, uint8_t controller, uint8_t channel)  // Constructor
  : pin(pin), controller(controller), channel(channel) {
  ExtIO::pinMode(pin, INPUT_PULLUP); // Enable the internal pull-up resistor on the pin with the button/switch
}

DigitalCC::~DigitalCC() {  // Destructor 
  ExtIO::pinMode(pin, INPUT);  // make it a normal input again, without the internal pullup resistor.
}

void DigitalCC::invert()  // Invert the button state
{
  invertState = true;
}

void DigitalCC::refresh() {  // Check if the button state changed, and send a MIDI CC accordingly
  bool state = ExtIO::digitalRead(pin) ^ invertState;  // read the button state and invert it if "invert" is true

  if (millis() - prevBounceTime > debounceTime)
  {
    int8_t stateChange = state - buttonState;

    if (stateChange == falling)
    { // Button is pushed
      buttonState = state;
      MIDI_Controller.MIDI()->send(CONTROL_CHANGE, 
        channel + channelOffset * channelsPerBank, 
        controller + addressOffset * channelsPerBank, 127);
    }

    if (stateChange == rising)
    { // Button is released
      buttonState = state;
      MIDI_Controller.MIDI()->send(CONTROL_CHANGE, 
        channel + channelOffset * channelsPerBank,
        controller + addressOffset * channelsPerBank, 0);
    }
  }
  if (state != prevState)
  {
    prevBounceTime = millis();
    prevState = state;
  }
}