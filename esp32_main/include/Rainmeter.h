#pragma once

#include "Component.h"

/*
Digital output = 1 - 3 V independent from source
*/

#define RAINMETER_PIN 1// Rainmeter sensor data pin
#define RAIN_KEY "chuva"// JSON rain key

// MH-RD rain sensor
class Rainmeter : public Component{
  private:
    uint8_t rainmeter_data = 0;
    
  public:
    Rainmeter();// Create object
    ~Rainmeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};