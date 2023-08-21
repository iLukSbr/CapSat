/*
MIT License

Copyright (c) 2023 Lucas Yukio Fukuda Matsumoto

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#define CALIBRATION_DELAY 1000// Delay between calibration requests (ms)
#define SERIAL_BAUD_RATE 230400// Serial baud rate
#ifdef ESP32// For ESP32
  #define PIN_MAX_VOLTAGE 3.3
  #define PIN_MAX_ANALOG 4095
#elif defined(ESP8266)// For ESP8266
  #define PIN_MAX_VOLTAGE 3.3
  #define PIN_MAX_ANALOG 4095
#else// For Arduino
  #define PIN_MAX_VOLTAGE 5.f
  #define PIN_MAX_ANALOG 1023
#endif