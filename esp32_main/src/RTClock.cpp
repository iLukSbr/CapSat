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

#include "pch.h"
#include "RTClock.h"

RTClock::RTClock(const uint16_t& year, const uint8_t& month, const uint8_t& day, const uint8_t& hour, const uint8_t& minute, const uint8_t& second):
    rtc(new RTC_DS3231()),// Instatiate RTC
    day_of_the_week(nullptr)
{// Create object
    start();
    if(isStarted())
        rtcAdjust(year, month, day, hour, minute, second);
}

RTClock::RTClock():
    rtc(new RTC_DS3231()),// Instatiate RTC
    day_of_the_week(nullptr)
{// Create object
    start();
}

RTClock::~RTClock(){// Release memory
    delete rtc;
}

void RTClock::gatherData(){// Get data from component
    multiPrintln(F("Gathering RTClock DS3231 data..."));
    DateTime now = rtc->now();// Get real time
    day_of_the_week = days[now.dayOfTheWeek()];
    snprintf_P(clock_data,
               21,
               PSTR("%02u-%02u-%04u_%02u-%02u-%02u"),// Date-hour format
               now.day(),// Day
               now.month(),// Month
               now.year(),// Year
               now.hour(),// Hour
               now.minute(),// Minutes
               now.second());// Seconds
}

void RTClock::printData(){// Display data for test
    multiPrint(F("RTClock DS3231: "));
    multiPrint(clock_data);
    multiPrintln();
}

void RTClock::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    if(!isHTTP){
        doc[F(DATE_TIME_KEY)] = clock_data;
        payload[F(DAY_OF_THE_WEEK_KEY)] = day_of_the_week;
    }
}

void RTClock::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(clock_data);
    my_file->print(F(","));
}

char* RTClock::getDateTime(){// Date and time
    gatherData();
    return clock_data;
}

bool RTClock::checkValidDate(const uint16_t& minute){// Check if date is valid
    if(rtc->now().day()>0 && rtc->now().minute()==minute)
        return true;
    return false;
}

void RTClock::rtcAdjust(const uint16_t& year, const uint8_t& month, const uint8_t& day, const uint8_t& hour, const uint8_t& minute, const uint8_t& second){
    rtc->adjust(DateTime(year, month, day, hour, minute, second));
}

void RTClock::start(){
    Serial.println(F("Starting RTC DS3231..."));
    for(byte i=0; i<START_TRIES; i++){
        if(rtc->begin()){
            rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));
            started = true;
            Serial.println(F("RTClock DS3231 OK!"));
            break;
        }
        Serial.println(F("Waiting for RTC DS3231..."));
        delay(CALIBRATION_DELAY);
    }
}