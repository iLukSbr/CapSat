/*
MIT License

Copyright (c) 2023 Lucas Yukio Fukuda Matsumoto

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
#define OZONOSCOPE_DECIMAL_PLACES 4// Concentration decimal places
#define OZONOSCOPE_NAME "MQ-131"// MQ-131 sensor name
#define O3_KEY "O3"// JSON ozone concentration key

#ifdef ESP32
  #define OZONOSCOPE_BOARD "ESP32"// For ESP32
  #define ADC_RESOLUTION 12// 12 bits
#elif defined(ESP8266)
  #define OZONOSCOPE_BOARD "ESP8266"// For ESP8266
  #define ADC_RESOLUTION 10// 10 bits
#else
  #define OZONOSCOPE_BOARD "Arduino"// For Arduino
  #define ADC_RESOLUTION 10// 10 bits
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