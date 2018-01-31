#ifndef USBMIDI_INTERFACE_H_
#define USBMIDI_INTERFACE_H_

#include "MIDI_Interface.h"

#ifdef CORE_TEENSY
// #define USB_MIDI // Doesn't work
// #include <usb_dev.h>
#include "usb_api.h"
#include "usb_private.h"
#endif

#if defined(USBCON) && !defined(CORE_TEENSY) // If the main MCU has a USB connection but is not a Teensy
#include "MIDIUSB.h"
#endif

#if defined(USBCON) || defined(CORE_TEENSY) // If the main MCU has a USB connection (or is a Teensy)
class USBMIDI_Interface : public MIDI_Interface
{
  public:
    USBMIDI_Interface()
    {
        ;
    }

    bool refresh() // Ignore MIDI input
    {
#if defined(CORE_TEENSY) // If it's a Teensy board
        return usbMIDI.read();
#elif defined(USBCON) // If the main MCU has a USB connection but is not a Teensy
        return MidiUSB.read().header != 0; // if there's a packet to read, discard it, and read again next time
#endif
    }

  protected:
    void sendImpl(uint8_t m, uint8_t c, uint8_t d1, uint8_t d2)
    {
#if defined(CORE_TEENSY) // If it's a Teensy board
    uint8_t intr_state, timeout;

    if (!usb_configuration)
        return;
    intr_state = SREG;
    cli();
    UENUM = MIDI_TX_ENDPOINT;
    timeout = UDFNUML + 2;
    while (1)
    { // are we ready to transmit?
        if (UEINTX & (1 << RWAL))
            break;
        SREG = intr_state;
        if (UDFNUML == timeout)
            return;
        if (!usb_configuration)
            return;
        intr_state = SREG;
        cli();
        UENUM = MIDI_TX_ENDPOINT;
    }
    UEDATX = m >> 4;
    UEDATX = m | c;
    UEDATX = d1;
    UEDATX = d2;
    if (!(UEINTX & (1 << RWAL)))
        UEINTX = 0x3A;
    SREG = intr_state;

#elif defined(USBCON) // If the main MCU has a USB connection but is not a Teensy
        midiEventPacket_t msg = {m >> 4, m | c, d1, d2};
        MidiUSB.sendMIDI(msg);
        MidiUSB.flush();
#endif
}
void sendImpl(uint8_t m, uint8_t c, uint8_t d1)
{
    sendImpl(m, c, d1, 0);
}
    };

#else // If the main MCU doesn't have a USB connection

#include "SerialMIDI_Interface.h"

class USBMIDI_Interface : public USBSerialMIDI_Interface
{
  public:
    USBMIDI_Interface() : USBSerialMIDI_Interface(MIDI_BAUD){};
};

#endif

#endif // USBMIDI_INTERFACE_H_