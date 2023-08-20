#pragma once

#include "Component.h"

#define RAINMETER_PIN 1// Rainmeter sensor data pin

// MH-RD rainmeter
class Rainmeter : public Component{
  private:
  public:
    uint8_t rainmeter_data = 0;
    void begin() override{
      pinMode(RAINMETER_PIN, INPUT);// Defines RAINMETER_PIN as input
    }
    void gatherData() override{
      digitalRead(RAINMETER_PIN) == HIGH ? rainmeter_data=0 : rainmeter_data=1;
    }// HIGH, 0 = dry; LOW, 1 = wet
    void printData() override{// Display data for test
      Serial.print(F("Sensor de chuva: "));
      Serial.print(rainmeter_data);
      Serial.println();
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(rainmeter_data);
      my_file->print(F(","));
    }
};