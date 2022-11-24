//
// config_arduino.h
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

#define NOT_CONNECTED NA

#define DIGITAL_PIN_00 0
#define DIGITAL_PIN_01 1
#define DIGITAL_PIN_02 2
#define DIGITAL_PIN_03 3
#define DIGITAL_PIN_04 4
#define DIGITAL_PIN_05 5
#define DIGITAL_PIN_06 6
#define DIGITAL_PIN_07 7
#define DIGITAL_PIN_08 0
#define DIGITAL_PIN_09 1
#define DIGITAL_PIN_10 2
#define DIGITAL_PIN_11 3
#define DIGITAL_PIN_12 4

#define ANALOG_PIN_00 0
#define ANALOG_PIN_01 1
#define ANALOG_PIN_02 2

#define PORTB_PIN_OFFSET 8

#define NES_LATCH DIGITAL_PIN_03
#define NES_CLOCK DIGITAL_PIN_06
#define NES_DATA DIGITAL_PIN_04
#define NES_DATA0 DIGITAL_PIN_02
#define NES_DATA1 DIGITAL_PIN_05

#define PIND_READ(pin) (PIND & (1 << (pin)))
#define PINB_READ(pin) (PINB & (1 << (pin)))
#define PINC_READ(pin) (PINC & (1 << (pin)))

#define READ_PORTD(mask) (PIND & mask)
#define READ_PORTB(mask) (PINB & mask)

#define T_DELAY(ms) delay(0)
#define A_DELAY(ms) delay(ms)

#define FASTRUN
