#pragma once

#include "Component.h"

// MiCS-6814 gas meter
// https://github.com/noorkhokhar99/MICS6814
#include <MICS6814.h>

#define GAS_METER_CO_PIN 1// Gas meter analog CO pin
#define GAS_METER_NO2_PIN 2// Gas meter analog NO2 pin
#define GAS_METER_NH3_PIN 3// Gas meter analog NH3 pin
#define GAS_METER_SIZE 8// Sensor data quantity
#define GAS_DECIMAL_PLACES 4// Decimal places for gas concentration
#define GAS_METER_KEY "gasometro"// JSON gas meter key
#define NH3_KEY "NH3"// JSON ammonia key
#define CO_KEY "CO"// JSON carbon monoxide key
#define NO2_KEY "NO2"// JSON nitrogen dioxide key
#define C3H8_KEY "C3H8"// JSON propane concentration key key
#define C4H10_KEY "C4H10"// JSON butane concentration key
#define CH4_KEY "CH4"// JSON methane concentration key
#define H2_KEY "H2"// JSON hydrogen gas concentration key
#define C2H5OH_KEY "C2H5OH"// JSON ethanol concentration key

class GasMeter : public Component{
  private:
    MICS6814* gas = nullptr;
    float gas_meter_data[GAS_METER_SIZE] = {0.f};
    
  public:
    GasMeter();// Create object
    ~GasMeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};