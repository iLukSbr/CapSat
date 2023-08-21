#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// INA219 multimeter
// https://github.com/wollewald/INA219_WE
#include <INA219_WE.h>// Address 0x40

#define MULTIMETER_SIZE 3// Sensor data quantity
#define SHUNT_RESISTANCE 0.01// Shunt resistance 10R = 0.01K
#define BATTERY_KEY "bateria"// JSON battery key
#define VOLTAGE_KEY "tensao"// JSON voltage key
#define CURRENT_KEY "corrente"// JSON electric current key
#define POWER_KEY "potencia"// JSON electric power key

class Multimeter : public Component{
  private:
    INA219_WE* mult = nullptr;
    float multimeter_data[MULTIMETER_SIZE] = {0.f};
    
  public:
    Multimeter();// Create object
    ~Multimeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};