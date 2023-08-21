#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// ENS160+AHT21 carbodioximeter, TVOC (total volatile organic compounds) meter and humidimeter
// https://github.com/DFRobot/DFRobot_ENS160
// https://github.com/enjoyneering/AHTxx
#include <DFRobot_ENS160.h>// Address 0x53
#include <AHTxx.h>// Address 0x38

#define HUMIDIMETER_SIZE 4// Sensor data quantity
#define HUMIDITY_KEY "umidade"// JSON humidity key
#define TVOC_KEY "TCOV"// JSON Total Volatile Organic Compounds (TVOC) key
#define CO2_KEY "CO2"// JSON CO2 key
#define AQI_KEY "IQA"// JSON Air Quality Index (AQI) key

class Humidimeter : public Component{
  private:
    AHTxx* aht21 = nullptr;
    DFRobot_ENS160_I2C* ens160 = nullptr;
    float humidimeter_data[HUMIDIMETER_SIZE] = {0.f};
    
  public:
    Humidimeter();// Create object
    ~Humidimeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};