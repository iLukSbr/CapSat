#pragma once

#include "Component.h"

// Taidacent UV radiometer

/*
Analog output = 0 - 1 V independent from source
*/

#define UV_RADIOMETER_PIN 7// UV radiometer analog pin
#define UV_KEY "UV"// JSON UV level key

class UVRadiometer : public Component{
  private:
    uint8_t uv_radiometer_data = 0;
    
  public:
    UVRadiometer();// Create object
    ~UVRadiometer();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};