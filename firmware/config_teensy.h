//
// config_teensy.h
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

#define NOT_CONNECTED 0

// PORT D
#define DIGITAL_PIN_02 0
#define DIGITAL_PIN_14 1
#define DIGITAL_PIN_07 2
#define DIGITAL_PIN_08 3
#define DIGITAL_PIN_06 4
#define DIGITAL_PIN_20 5
#define DIGITAL_PIN_21 6
#define DIGITAL_PIN_05 7
// PORT B
#define DIGITAL_PIN_16 0
#define DIGITAL_PIN_27 1
#define DIGITAL_PIN_19 2
#define DIGITAL_PIN_18 3

#define NES_LATCH DIGITAL_PIN_08
#define NES_CLOCK DIGITAL_PIN_21
#define NES_DATA DIGITAL_PIN_06
#define NES_DATA0 DIGITAL_PIN_07
#define NES_DATA1 DIGITAL_PIN_20

#define READ_PORTD(mask) (GPIOD_PDIR & mask)
#define READ_PORTB(mask) (GPIOB_PDIR & mask)

#define PIND_READ(pin) ((READ_PORTD(0xFF)) & (1 << (pin)))
#define PINB_READ(pin) ((READ_PORTB(0xF)) & (1 << (pin)))
#define PINC_READ(pin) (digitalReadFast(pin))

#define T_DELAY(ms) delay(ms)
#define A_DELAY(ms) delay(0)
