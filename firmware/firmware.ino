///////////////////////////////////////////////////////////////////////////////
// RetroSpy Firmware for Arduino Uno & Teensy 3.5
// v4.4
// RetroSpy written by zoggins of RetroSpy Technologies
// NintendoSpy originally written by jaburns

#include "common.h"

unsigned char rawData[NES_BITCOUNT * 3];

void setup()
{
    // for MODE_DETECT
#if defined(__arm__) && defined(CORE_TEENSY)
    for (int i = 33; i < 40; ++i)
        pinMode(i, INPUT_PULLUP);
#else
#if !defined(MODE_ATARI_PADDLES) && !defined(MODE_ATARI5200_1) && !defined(MODE_ATARI5200_2) && !defined(MODE_AMIGA_ANALOG_1) && !defined(MODE_AMIGA_ANALOG_2)
    PORTC = 0xFF; // Set the pull-ups on the port we use to check operation mode.
    DDRC = 0x00;
#endif
#endif

    Serial.begin(115200);
    common_pin_setup();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
    T_DELAY(5000);
    A_DELAY(200);
#pragma GCC diagnostic pop
}

void updateState()
{
    unsigned char bits = NES_BITCOUNT;
    unsigned char *rawDataPtr = rawData;

    WAIT_LEADING_EDGE(NES_LATCH);

    do
    {
        WAIT_FALLING_EDGE(NES_CLOCK);
        *rawDataPtr = !PIN_READ(NES_DATA);
        *(rawDataPtr + 8) = !PIN_READ(NES_DATA0);
        *(rawDataPtr + 16) = !PIN_READ(NES_DATA1);
        ++rawDataPtr;
    } while (--bits > 0);
}

void loop()
{
    updateState();
    sendRawData(rawData, NES_BITCOUNT * 3);
    T_DELAY(5);
}
