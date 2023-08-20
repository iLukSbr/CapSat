#pragma once

#include "Component.h"

// MH-RTC 2 clock
// https://github.com/Makuna/Rtc
#include <ThreeWire.h>
#include <RtcDS1302.h>

#define RTCLOCK_SIZE 21// Date and time string length
#define DATE_TIME_KEY "data-hora"// JSON date and time key

class RTClock : public Component{
  private:
    ThreeWire* myWire = nullptr;
    RtcDS1302<ThreeWire>* Rtc = nullptr;
    char clock_data[RTCLOCK_SIZE] = {0};
  public:
    RTClock(const uint16_t& year, const uint8_t month, const uint8_t day, const uint8_t hour, const uint8_t minute, const uint8_t second){
      myWire = new ThreeWire(6, 7, 5);// Instatiate pins
      Rtc = new RtcDS1302<ThreeWire>(*myWire);// Instatiate RTC
      if(Rtc->GetIsWriteProtected())// If RTC is read only
        Rtc->SetIsWriteProtected(false);// Enable RTC write
      if(!Rtc->GetIsRunning())// If RTC is off
        Rtc->SetIsRunning(true);// Start RTC
      Rtc->SetDateTime(RtcDateTime(year, month, day, hour, minute, second));// Update date and time
      Rtc->SetIsWriteProtected(true);// Disable RTC write
    }
    ~RTClock(){
      delete myWire;
      delete Rtc;
    }
    void gatherData() override{
      RtcDateTime datetime = Rtc->GetDateTime();
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
    void printData() override{// Display data for test
      Serial.print(F("Relógio: "));
      Serial.print(clock_data);
      Serial.println();
    }
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
      if(!isHTTP)
        payload[F(DATE_TIME_KEY)] = clock_data;
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(clock_data);
      my_file->print(F(","));
    }
    const char* getDateTime(){
      return clock_data;
    }
};