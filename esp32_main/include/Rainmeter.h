#pragma once

#include "Component.h"

#define RAINMETER_PIN 1// Rainmeter sensor data pin
#define RAIN_KEY "chuva"// JSON rain key

// MH-RD rainmeter
class Rainmeter : public Component{
  private:
    uint8_t rainmeter_data = 0;
  public:
    Rainmeter(){
      pinMode(RAINMETER_PIN, INPUT);// Defines RAINMETER_PIN as input
    }
    ~Rainmeter(){
      pinMode(RAINMETER_PIN, OUTPUT);
      digitalWrite(RAINMETER_PIN, LOW);
    }
    void gatherData() override{
      digitalRead(RAINMETER_PIN) == HIGH ? rainmeter_data=0 : rainmeter_data=1;
    }// HIGH, 0 = dry; LOW, 1 = wet
    void printData() override{// Display data for test
      Serial.print(F("Sensor de chuva: "));
      Serial.print(rainmeter_data);
      Serial.println();
    }
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
      payload[F(RAIN_KEY)] = rainmeter_data;
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(rainmeter_data);
      my_file->print(F(","));
    }
};