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

// MiCS-6814 gas meter
// https://github.com/noorkhokhar99/MICS6814
#include <MICS6814.h>

// ADS1115 ADC 16 bits
// https://github.com/wollewald/ADS1115_WE
#include <ADS1115_WE.h>

#define ADC_I2C_ADDRESS 0x48// ADS1115 I²C address
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
    ADS1115_WE* adc;
    MICS6814* gas;
    float gas_meter_data[GAS_METER_SIZE] = {0.f};
    
  public:
    GasMeter();// Create object
    ~GasMeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};