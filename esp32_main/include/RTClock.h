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

#include "Component.h"

// MH-RTC 2 DS1302 clock
// https://github.com/Makuna/Rtc
#include <ThreeWire.h>
#include <RtcDS1302.h>

#define RTCLOCK_DAT_PIN 1// RTC I/O data pin
#define RTCLOCK_CLK_PIN 2// RTC clock pin
#define RTCLOCK_RST_PIN 3// RTC CE reset pin
#define RTCLOCK_SIZE 21// Date and time string length
#define DATE_TIME_KEY "id"// JSON date and time key

class RTClock : public Component{
  private:
    ThreeWire* myWire;
    RtcDS1302<ThreeWire>* Rtc;
    char clock_data[RTCLOCK_SIZE] = {0};
    
  public:
    RTClock(const uint16_t& year, const uint8_t month, const uint8_t day, const uint8_t hour, const uint8_t minute, const uint8_t second);// Create object
    ~RTClock();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    const char* getDateTime() const;// Date and time
};