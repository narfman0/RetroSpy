//
// common.cpp
//
// Author:
//       Christopher "Zoggins" Mallery <zoggins@retro-spy.com>
//
// Copyright (c) 2020 RetroSpy Technologies
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "common.h"

void common_pin_setup()
{
#if defined(__arm__) && defined(CORE_TEENSY)
	// GPIOD_PDIR & 0xFF;
	pinMode(2, INPUT_PULLUP);
	pinMode(14, INPUT_PULLUP);
	pinMode(7, INPUT_PULLUP);
	pinMode(8, INPUT_PULLUP);
	pinMode(6, INPUT_PULLUP);
	pinMode(20, INPUT_PULLUP);
	pinMode(21, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP);

	// GPIOB_PDIR & 0xF;
	pinMode(16, INPUT_PULLUP);
	pinMode(17, INPUT_PULLUP);
	pinMode(19, INPUT_PULLUP);
	pinMode(18, INPUT_PULLUP);
#else
	PORTD = 0x00;
	PORTB = 0x00;
	DDRD = 0x00;

	for (int i = 2; i <= 6; ++i)
		pinMode(i, INPUT_PULLUP);
#endif
}


unsigned long lastSendMs = 0;
unsigned long lastResetMs = 0; // latch always happens in smb3 within 1 second or so

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sends a packet of controller data over the Arduino serial interface.
#pragma GCC optimize("-O2")
#pragma GCC push_options
void sendRawData(unsigned char rawControllerData[], unsigned char first, unsigned char count)
{
  unsigned long currentMs = millis();
  if(currentMs - lastSendMs > 2000){
    lastResetMs = currentMs;
  }
  lastSendMs = currentMs;
	for (unsigned char i = first; i < first + count; i++)	{
		Serial.write(rawControllerData[i] ? ONE : ZERO);
	}
  unsigned long msSinceReset = currentMs - lastResetMs;
  for (int i = 0; i < 32; i+=8) {
    Serial.write((msSinceReset >> i) & 0xff);
  }
	Serial.write(SPLIT);
}
#pragma GCC pop_options
