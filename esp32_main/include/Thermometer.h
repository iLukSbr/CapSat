#pragma once

#include "Component.h"

// NTC thermometer
// https://github.com/YuriiSalimov/NTC_Thermistor
#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>

#define TEMPERATURE_KEY "temperatura"// JSON temperature key

class Thermometer : public Component{
  private:
    Thermistor* originThermistor = nullptr;
    Thermistor* thermistor = nullptr;
    float thermometer_data = 0.f;
  public:
    Thermometer(){
      originThermistor = new NTC_Thermistor(56, 9870, 10000, 25, 3950);
      thermistor = new SmoothThermistor(originThermistor, 5);
    }
    ~Thermometer(){
      delete originThermistor;
      delete thermistor;
    }
    void gatherData() override{// Leitura da tensão e conversão para graus Celsius
      thermometer_data = thermistor->readCelsius();
    }
    void printData() override{// Display data for test
      Serial.print(F("Termômetro: "));
      Serial.print(thermometer_data);
      Serial.println();
    }
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
      doc[F(TEMPERATURE_KEY)] = thermometer_data;
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(thermometer_data);
      my_file->print(F(","));
    }
};