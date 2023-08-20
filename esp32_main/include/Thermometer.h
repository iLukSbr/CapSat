#pragma once

#include "Component.h"

// NTC thermometer
// https://github.com/YuriiSalimov/NTC_Thermistor
#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>

class Thermometer : public Component{
  private:
    Thermistor* originThermistor = nullptr;
    Thermistor* thermistor = nullptr;
  public:
    float thermometer_data;
    void begin() override{
      originThermistor = new NTC_Thermistor(56, 9870, 10000, 25, 3950);
      thermistor = new SmoothThermistor(originThermistor, 5);
    }
    void gatherData() override{// Leitura da tensão e conversão para graus Celsius
      thermometer_data = thermistor->readCelsius();
    }
    void printData() override{// Display data for test
      Serial.print(F("Termômetro: "));
      Serial.print(thermometer_data);
      Serial.println();
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(thermometer_data);
      my_file->print(F(","));
    }
};