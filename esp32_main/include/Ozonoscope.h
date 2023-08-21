#pragma once

#include "Component.h"

// Winsen MQ-131 low concentration ozone sensor
// https://github.com/miguel5612/MQSensorsLib
#include <MQUnifiedsensor.h>

/*
VIN = 5 V
Analog output = 0 - 5 V (need level shifter)
*/

#define OZONOSCOPE_PIN 6// Ozonoscope analog pin
#define OZONOSCOPE_A_PARAMETER 23.943// Configure the equation to to calculate O3 concentration
#define OZONOSCOPE_B_PARAMETER -1.11// Configure the equation to to calculate O3 concentration
#define OZONOSCOPE_REGRESSION_METHOD 1// Set regression method a*ratio^b
#define OZONOSCOPE_OFFSET 0.1// Value offset
#define OZONOSCOPE_CLEAN_AIR_RATIO 15// Clean air ratio RS/R0 = 15 ppm
#define OZONOSCOPE_CALIBRATION_LOOP 10// How many times MQ-131 will calibrate
#define OZONOSCOPE_VOLTAGE_RESOLUTION 5// MQ-131 voltage resolution
#define OZONOSCOPE_ADC_BIT_RESOLUTION 10// MQ-131 ADC bit resolution
#define OZONOSCOPE_DECIMAL_PLACES 4// Concentration decimal places
#define OZONOSCOPE_NAME "MQ-131"// MQ-131 sensor name
#define O3_KEY "O3"// JSON ozone concentration key

#ifdef ESP32
  #define OZONOSCOPE_BOARD "ESP32"// For ESP32
#elif defined(ESP8266)
  #define OZONOSCOPE_BOARD "ESP8266"// For ESP8266
#else
  #define OZONOSCOPE_BOARD "Arduino"// For Arduino
#endif

class Ozonoscope : public Component{
  private:
    MQUnifiedsensor* MQ131 = nullptr;
    float ozonoscope_data = 0.f;
    
  public:
    Ozonoscope();// Create object
    ~Ozonoscope();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};