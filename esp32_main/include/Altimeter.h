#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// MS5611 altimeter/barometer
// https://github.com/RobTillaart/MS5611
#include <MS5611.h>// Address 0x77

#define ALTIMETER_SIZE 2// Sensor data quantity
#define MS5611_PRESSURE_OFFSET -450// Pressure offset calibration according to local air pressure (Pa)
#define PRESSURE_KEY "pressao"// JSON pressure key
#define ALTITUDE_KEY "altitude"// JSON altitude key

class Altimeter : public Component{
  private:
    MS5611* baro = nullptr;
    float altimeter_data[ALTIMETER_SIZE] = {0.f};
    
  public:
    Altimeter();// Create object
    ~Altimeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};