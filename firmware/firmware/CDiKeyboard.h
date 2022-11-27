//
// CDiKeyboard.h
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

#ifndef CDiKeyboard_h
#define CDiKeyboard_h

#include "ControllerSpy.h"

#if !defined(TP_PINCHANGEINTERRUPT) && !(defined(__arm__) && defined(CORE_TEENSY))

#include <SoftwareSerial.h>

class CDiKeyboardSpy : public ControllerSpy {
public:
	CDiKeyboardSpy()
		: vSerial(9, 10, true),
		T_mode_caps_on(false)
	{}
	
	void setup();
	void loop();
	void writeSerial();
	void debugSerial();
	void updateState();
	
private:
	SoftwareSerial vSerial;
	byte rawData[10];
	byte incomingBytes[4];
	bool T_mode_caps_on;
};
#else
class CDiKeyboardSpy : public ControllerSpy {
public:
	CDiKeyboardSpy(){}
	void setup() {}
	void loop() {}
	void writeSerial() {}
	void debugSerial() {}
	void updateState() {}
};
#endif
#endif