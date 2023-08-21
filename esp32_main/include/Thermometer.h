#pragma once

#include "Component.h"

// KY-013 NTC temperature sensor
// https://github.com/YuriiSalimov/NTC_Thermistor
#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>

/*
ESP: VIN = 3.3 V
Analog output = 0 - 3.3 V

Arduino: VIN = 5 V
Analog output = 0 - 5 V
*/

#define NTC_SENSOR_PIN 5
#define NTC_REFERENCE_RESISTANCE 9870
#define NTC_NOMINAL_RESISTANCE 10000
#define NTC_NOMINAL_TEMPERATURE 25
#define NTC_B_VALUE 3950
#define NTC_SMOOTHING_FACTOR 5
#define TEMPERATURE_KEY "temperatura"// JSON temperature key

class Thermometer : public Component{
  private:
    Thermistor* originThermistor = nullptr;
    Thermistor* thermistor = nullptr;
    float thermometer_data = 0.f;
    
  public:
    Thermometer();// Create object
    ~Thermometer();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};