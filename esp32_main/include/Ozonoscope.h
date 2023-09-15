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
// https://github.com/ostaquet/Arduino-MQ131-driver
#include <MQ131.h>

// ADS1115 ADC 16 bits
// https://github.com/wollewald/ADS1115_WE
#include <ADS1115_WE.h>

/*
VIN = 5 V
Analog output = 0 - 4.2 V (need level shifter)
*/

#define ADC_I2C_ADDRESS 0x48// ADS1115 I²C address
#define OZONOSCOPE_HEATER_PIN 33// Ozonoscope heater digital pin
#define OZONOSCOPE_CALIBRATION_CYCLE 1// Number of stable cycles to consider calibrated
#define OZONOSCOPE_RL 1000000// 1M heater resistor (Ohm)
#define O3_KEY "O3"// JSON ozone concentration key

class Ozonoscope : public Component{
  private:
    float ozonoscope_data = 0;
    ADS1115_WE* adc;
    
  public:
    Ozonoscope();// Create object
    ~Ozonoscope();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    void setClimateParameters(float temperature, float humidity);// Define climate parameters
};