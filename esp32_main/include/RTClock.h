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
    RTClock(const uint16_t& year, const uint8_t month, const uint8_t day, const uint8_t hour, const uint8_t minute, const uint8_t second);// Create object
    ~RTClock();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    const char* getDateTime() const;// Date and time
};