#pragma once

#include "Component.h"

// MH-RTC 2 clock
// https://github.com/Makuna/Rtc
#include <ThreeWire.h>
#include <RtcDS1302.h>

class RTClock : public Component{
  private:
    ThreeWire* myWire = nullptr;// Instatiate pins
    RtcDS1302<ThreeWire>* Rtc = nullptr;
// Instatiate RTC
  public:
    char clock_data[21] = {0};
    void begin() override{
      myWire = new ThreeWire(6, 7, 5);
      Rtc = new RtcDS1302<ThreeWire>(*myWire);
      if(Rtc->GetIsWriteProtected())// If RTC is read only
        Rtc->SetIsWriteProtected(false);// Enable RTC write
      if(!Rtc->GetIsRunning())// If RTC is off
        Rtc->SetIsRunning(true);// Start RTC
      //Rtc->SetDateTime(RtcDateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour()-3, gps.time.minute(), gps.time.second()));// VARIÁVEL RECEBE DATA E HORA DE COMPILAÇÃO
      Rtc->SetIsWriteProtected(true);// Disable RTC write
    }
    void gatherData() override{
      RtcDateTime datetime = Rtc->GetDateTime();
      snprintf_P(clock_data,
                 21,
                 PSTR("%02u-%02u-%04u_%02u-%02u-%02u"),// Date-hour format
                 datetime.Day(),// DIA
                 datetime.Month(),// MÊS
                 datetime.Year(),// ANO
                 datetime.Hour(),// HORA
                 datetime.Minute(),// MINUTOS
                 datetime.Second());// SEGUNDOS
    }
    void printData() override{// Display data for test
      Serial.print(F("Relógio: "));
      Serial.print(clock_data);
      Serial.println();
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(clock_data);
      my_file->print(F(","));
    }
};