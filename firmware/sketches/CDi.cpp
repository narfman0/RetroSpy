﻿//
// CDi.cpp
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

#include "CDi.h"

#if defined(TP_IRLIB2) && !(defined(__arm__) && defined(CORE_TEENSY))

static unsigned long wireless_timeout;
static unsigned long wireless_remote_timeout;
static byte wireless_rawData[15];
static byte wireless_yaxis;
static byte wireless_xaxis;

static byte max_right;
static byte max_left;
static byte max_up;
static byte max_down;

static byte wired_rawData[6];
static byte wired_yAxis;
static byte wired_xAxis;
static unsigned long wired_timeout;

void CDiSpy::setup() {
	vSerial.begin(1200); 
	wired_rawData[4] = wired_rawData[5] = 0;

	max_right = max_left = max_up = max_down = 0;
  
	myReceiver.enableIRIn();  // Start the receiver
#ifdef WIRELESS_DEBUG
	  Serial.println(F("Ready to receive IR signals"));
#endif
	wireless_timeout = wired_timeout = wireless_remote_timeout = millis();
}

void CDiSpy::loop() {
	HandleSerial();
	HandleIR();
#ifndef WIRED_DEBUG
#ifndef WIRELESS_DEBUG
	printRawData();
#endif
#endif
}
void CDiSpy::writeSerial() {}
void CDiSpy::debugSerial() {}
void CDiSpy::updateState() {}

void CDiSpy::HandleSerial()
{

	if (vSerial.available() >= 3) {

		char c = vSerial.read();
		if ((c & 0b11000000) == 0b11000000)
		{
   
#ifndef WIRED_DEBUG     
  
			wired_rawData[4] = (c & 0b00100000) != 0 ? 1 : 0;   // Button 1
			wired_rawData[5] = (c & 0b00010000) != 0 ? 1 : 0;   // Button 2
  
			 wired_yAxis = ((c & 0b00001100) << 4);
			wired_xAxis = ((c & 0b00000011) << 6);
  
			c = vSerial.read();
			wired_xAxis = wired_xAxis + (byte)(c & 0b00111111);
  
			c = vSerial.read();
			wired_yAxis = wired_yAxis + (byte)(c & 0b00111111);
  
			if (wired_yAxis == 0 || wired_yAxis == 128)
			{
				wired_rawData[0] = 0;
				wired_rawData[1] = 0;
			}
			else if (wired_yAxis > 128)
			{
				if (wired_yAxis > max_up)
					max_up = wired_yAxis;
          
				wired_rawData[0] =  ScaleInteger(256 - wired_yAxis + 128, 128, max_up, 0, 255);
				wired_rawData[1] = 0;
			}
			else
			{
				if (wired_yAxis > max_down)
					max_down = wired_yAxis;
          
				wired_rawData[1] =  ScaleInteger(wired_yAxis, 0, max_down, 0, 255);
				wired_rawData[0] = 0;
			}
  
			if (wired_xAxis == 0 || wired_xAxis == 128)
			{
				wired_rawData[2] = 0;
				wired_rawData[3] = 0;
			}
			else if (wired_xAxis > 128)
			{
				if (wired_xAxis > max_left)
					max_left = wired_xAxis;
             
				wired_rawData[2] = ScaleInteger(256 - wired_xAxis + 128, 128, max_left, 0, 255);
				wired_rawData[3] = 0;
			}
			else
			{
				if (wired_xAxis > max_right)
					max_right = wired_xAxis;
             
				wired_rawData[3] = ScaleInteger(wired_xAxis, 0, max_right, 0, 255);
				wired_rawData[2] = 0;
			}
  
			wired_timeout = millis();
#else     
			for (int i = 7; i >= 0; --i)
				Serial.print((c & (1 << i)) ? "1" : "0");
			Serial.print("|");
       
			for (int i = 0; i < 2; ++i)
			{
				char c = vSerial.read();
				for (int i = 7; i >= 0; --i)
					Serial.print((c & (1 << i)) ? "1" : "0");
				if (i == 1)
					Serial.print("\n");
				else
					Serial.print("|");
			}
#endif
		}
	}
	else if (((millis() - wired_timeout) >= _wired_timeout))
	{
		for (int i = 0; i < 4; ++i)
			wired_rawData[i] = 0;
		wired_xAxis = wired_yAxis = 0;

	}
}

void CDiSpy::HandleIR()
{
	if (myReceiver.getResults()) {
		myDecoder.decode();    //Decode it

	  if(myDecoder.bits == 29)
		{
			myDecoder.value = myDecoder.value >> 1;
		}
    
		if ((myDecoder.protocolNum == 13
		    && myDecoder.address == 0 
		    && (myDecoder.bits == 29 && (myDecoder.value & 0b11111111111100000000000000000000) == 0b00001001001000000000000000000000))
			|| (myDecoder.protocolNum == 4 && myDecoder.address == 0 && myDecoder.bits == 20)
			)                     
		{
#ifdef WIRELESS_DEBUG
			for (int32_t i = 31; i >= 0; --i)
			{
				Serial.print((myDecoder.value & ((int32_t)1 << i)) != 0 ? "1" : "0");
				if (i % 8 == 0)
					Serial.print("|");
			}
			Serial.print("\n");
		}
#else
		char button_pushed = 0;
		for (int i = 0; i < 8; ++i)
		{
			button_pushed |= (myDecoder.value & (1 << i));
		}

		switch (button_pushed)
		{
		case 0x03: // Pause
		  wireless_rawData[6] = 1;
			break;
		case 0x02: // Stop
		  wireless_rawData[8] = 1;
			break;
		case 0x014: // Previous Track
		  wireless_rawData[9] = 1;
			break;
		case 0x04: // Play
		  wireless_rawData[10] = 1;
			break;
		case 0x10: // Next Track
			Serial.println(myDecoder.value, BIN);
			if((myDecoder.value & 0b00001111111100000000) == 0b00000001100100000000)
				wireless_rawData[14] = 1;
			else
				wireless_rawData[11] = 1;
			break;
		case 0x17: // TV/CDI
		  wireless_rawData[12] = 1;
			break;
		case 0x18: // Volume Down
		  wireless_rawData[13] = 1;
			break;
		case 0x19: // Volume Up
		  wireless_rawData[14] = 1;
			break;
			
		case 0x30: // Pause
		  wireless_rawData[6] = 1;
			break;
		case 0x0C: // Standby
		  wireless_rawData[7] = 1;
			break;
		case 0x31: // Stop
		  wireless_rawData[8] = 1;
			break;
		case 0x21: // Previous Track
		  wireless_rawData[9] = 1;
			break;
		case 0x2C: // Play
		  wireless_rawData[10] = 1;
			break;
		case 0x20: // Next Track
		  wireless_rawData[11] = 1;
			break;
		case 0x43: // TV/CDI
		  wireless_rawData[12] = 1;
			break;
		case 0x11: // Volume Down
		  wireless_rawData[13] = 1;
			break;
		}
		wireless_remote_timeout = millis();
	}

#endif          
	if ((myDecoder.protocolNum == 13 || myDecoder.protocolNum == 4) 
	    && myDecoder.address == 0 
	    && (myDecoder.bits == 32 || (myDecoder.bits == 29 && (myDecoder.value & 0b11111111111100000000000000000000) != 0b00001001001000000000000000000000)))                       
	{
#ifdef WIRELESS_DEBUG

//		for (int32_t i = 31; i >= 0; --i)
//		{
//			Serial.print((myDecoder.value & ((int32_t)1 << i)) != 0 ? "1" : "0");
//			if (i % 8 == 0)
//				Serial.print("|");
//		}
//		Serial.print("\n");
	}
}
#else
wireless_yaxis = 0;
for (int i = 0; i < 8; ++i)
{
	wireless_yaxis |= (myDecoder.value & (1 << i));
}
          
if (wireless_yaxis == 0 || wireless_yaxis == 128)
{
	wireless_rawData[0] = 0;
	wireless_rawData[1] = 0;
}
else if (wireless_yaxis > 128)
{
	if (myDecoder.bits == 29)
		wireless_yaxis = 128 + 256 - wireless_yaxis;

	if (wireless_yaxis > max_up)
		max_up = wireless_yaxis;
          
	wireless_rawData[0] =  ScaleInteger(wireless_yaxis, 128, max_up, 0, 255);
	wireless_rawData[1] = 0;
}
else
{
	if (wireless_yaxis > max_down)
		max_down = wireless_yaxis;
          
	wireless_rawData[1] =  ScaleInteger(wireless_yaxis, 0, max_down, 0, 255);
	wireless_rawData[0] = 0;
}
        
wireless_xaxis = 0;
for (int i = 8; i < 16; ++i)
{
	wireless_xaxis |= ((myDecoder.value & (1 << i)) >> 8);
} 
        
if (wireless_xaxis == 0 || wireless_xaxis == 128)
{
	wireless_rawData[2] = 0;
	wireless_rawData[3] = 0;
}
else if (wireless_xaxis > 128)
{
	if (myDecoder.bits == 29)
		wireless_xaxis = 128 + 256 - wireless_xaxis;


	if (wireless_xaxis > max_left)
		max_left = wireless_xaxis;
            
	wireless_rawData[2] = ScaleInteger(wireless_xaxis, 128, max_left, 0, 255);
	wireless_rawData[3] = 0;
}
else
{
	if (wireless_xaxis > max_right)
		max_right = wireless_xaxis;
            
	wireless_rawData[3] = ScaleInteger(wireless_xaxis, 0, max_right, 0, 255);
	wireless_rawData[2] = 0;
}

wireless_rawData[5] = (myDecoder.value & 0b00000000000000100000000000000000) != 0 ? 1 : 0;
wireless_rawData[4] = (myDecoder.value & 0b00000000000000010000000000000000) != 0 ? 1 : 0;
                      
wireless_timeout = millis();
}

}

#endif
    
if (((millis() - wireless_timeout) >= _wireless_timeout))
{
	for (int i = 0; i < 6; ++i)
		wireless_rawData[i] = 0;
	wireless_xaxis = wireless_yaxis = 0;
}
  
if (((millis() - wireless_remote_timeout) >= _wireless_timeout))
{
	for (int i = 6; i < 15; ++i)
		wireless_rawData[i] = 0;
}
    
myReceiver.enableIRIn();       //Restart receiver
}

void CDiSpy::printRawData()
{
#ifdef WIRED_PRETTY_PRINT
	for (int i = 0; i < 6; ++i)
	{
		Serial.print(wired_rawData[i]);
		Serial.print("|"); 
	}

	Serial.print(wired_yAxis > 128 ? 256 - wired_yAxis + 128 : wired_yAxis);
	Serial.print("|"); 
	Serial.print(wired_xAxis > 128 ? 256 - wired_xAxis + 128 : wired_xAxis);
	Serial.print("\n"); 
#elif defined WIRELESS_PRETTY_PRINT
	for (int i = 0; i < 15; ++i)
	{
		Serial.print(wireless_rawData[i]);
		Serial.print("|"); 
	}
	Serial.print(wireless_yaxis);
	Serial.print("|"); 
	Serial.print(wireless_xaxis);
	Serial.print("\n");
#else
	int j = 0;
	for (int i = 0; i < 6; ++i)
	{
		Serial.write(wired_rawData[i] & 0xF0);
		Serial.write((wired_rawData[i] & 0x0F) << 4);
	}
	for (int i = 0; i < 15; ++i)
	{
		Serial.write(wireless_rawData[i] & 0xF0);
		Serial.write((wireless_rawData[i] & 0x0F) << 4);
	}
	Serial.write("\n");
#endif
}

#endif