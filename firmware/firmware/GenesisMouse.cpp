//
// GenesisMouse.cpp
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

#include "GenesisMouse.h"

#define WAIT_FOR_LINES_TO_SETTLE asm volatile (MICROSECOND_NOPS MICROSECOND_NOPS)

void GenesisMouseSpy::setup() {
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
	// Setup input pins
	// Assumes pin 8 is SELECT (DB9 pin 7)
	// Assumes pins 2-7 are DB9 pins 1,2,3,4,6,9
	// DB9 pin 5 is +5V !!!DO NOT CONNECT TO THE ARDUINO!!!!
	// DB9 pin 8 is ground (connecting probably won't hurt, but its unnecessary to connect it to the Arduino)
	for (byte i = 2; i <= 8; i++)
	{
		pinMode(i, INPUT_PULLUP);
	}
#endif
}

void GenesisMouseSpy::loop() {
	updateState();
#if !defined(DEBUG)
	writeSerial();
#else
	debugSerial();
#endif

	T_DELAY(5);
}

void GenesisMouseSpy::updateState() {
	unsigned long reads = 0;

	noInterrupts();

	WAIT_FALLING_EDGEB(GENESIS_TH);
	WAIT_FALLING_EDGE(GENESIS_TL);

	while (reads != 3) {
		rawData[reads] = 0;
		WAIT_FALLING_EDGE(GENESIS_TL);
		WAIT_FOR_LINES_TO_SETTLE;
		rawData[reads] |= (READ_PORTD(0b00111100) << 2);
		WAIT_LEADING_EDGE(GENESIS_TL);
		WAIT_FOR_LINES_TO_SETTLE;
		rawData[reads] |= (READ_PORTD(0b00111100) >> 2);
		++reads;
	}

	// This makes no sense.
	// Its like if there is no data the low nibble of the Y axis isn't sent.
	if (rawData[2] == 0b00001011)
		rawData[2] &= 0;

	interrupts();
}

void GenesisMouseSpy::writeSerial() {
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 8; ++j)
			Serial.write((rawData[i] & (1 << j)) == 0 ? ZERO : ONE);
	Serial.print("\n");
}

void GenesisMouseSpy::debugSerial() {
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 8; ++j)
			Serial.print((rawData[i] & (1 << j)) == 0 ? "0" : "1");
	Serial.print("\n");
}
