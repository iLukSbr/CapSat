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

// DS3231 RTC
// https://github.com/adafruit/RTClib
#include <RTClib.h>// I²C address 0x57 (EEPROM) and 0x68 (RTC)

#ifndef RTCLOCK
  #define RTCLOCK
#endif

#define RTCLOCK_SIZE 21// Date and time string length
#define DATE_TIME_KEY "id"// JSON date and time key
#define DAY_OF_THE_WEEK_KEY "dia da semana"// JSON day of the week key
#define PT_BR// Português brasileiro
// #define EN_US// Inglês

class RTClock : public Component{
  private:
    RTC_DS3231 *rtc;
    char clock_data[RTCLOCK_SIZE] = {0};
    char *day_of_the_week;
    #ifdef EN_US
      char days[7][14] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    #else
      char days[7][14] = {"Domingo", "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sábado"};
    #endif
    
  public:
    RTClock(const uint16_t& year, const uint8_t& month, const uint8_t& day, const uint8_t& hour, const uint8_t& minute, const uint8_t& second);// Create object
    RTClock();
    ~RTClock();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    char* getDateTime();// Date and time
    bool checkValidDate(const uint16_t& minute);// If date is valid
    void rtcAdjust(const uint16_t& year, const uint8_t& month, const uint8_t& day, const uint8_t& hour, const uint8_t& minute, const uint8_t& second);
    void start() override;
};