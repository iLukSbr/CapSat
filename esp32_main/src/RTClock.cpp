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

RTClock::RTClock(const uint16_t& year, const uint8_t month, const uint8_t day, const uint8_t hour, const uint8_t minute, const uint8_t second){// Create object
    myWire = new ThreeWire(6, 7, 5);// Instatiate pins
    Rtc = new RtcDS1302<ThreeWire>(*myWire);// Instatiate RTC
    if(Rtc->GetIsWriteProtected())// If RTC is read only
        Rtc->SetIsWriteProtected(false);// Enable RTC write
    if(!Rtc->GetIsRunning())// If RTC is off
        Rtc->SetIsRunning(true);// Start RTC
    Rtc->SetDateTime(RtcDateTime(year, month, day, hour, minute, second));// Update date and time
    Rtc->SetIsWriteProtected(true);// Disable RTC write
}
RTClock::~RTClock(){// Release memory
    delete myWire;
    delete Rtc;
}
void RTClock::gatherData(){// Get data from component
    RtcDateTime datetime = Rtc->GetDateTime();// Get real time
    snprintf_P(clock_data,
               21,
               PSTR("%02u-%02u-%04u_%02u-%02u-%02u"),// Date-hour format
               datetime.Day(),// Day
               datetime.Month(),// Month
               datetime.Year(),// Year
               datetime.Hour(),// Hour
               datetime.Minute(),// Minutes
               datetime.Second());// Seconds
}

void RTClock::printData(){// Display data for test
    Serial.print(F("Relógio: "));
    Serial.print(clock_data);
    Serial.println();
}

void RTClock::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    if(!isHTTP)
        doc[F(DATE_TIME_KEY)] = clock_data;
}

void RTClock::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(clock_data);
    my_file->print(F(","));
}

const char* RTClock::getDateTime() const{// Date and time
    return clock_data;
}