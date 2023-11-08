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

// I²C interface
#include <Wire.h>

// HW-291/GY-63 MS5611-01BA03 altimeter/barometer
// https://github.com/RobTillaart/MS5611
#include <MS5611.h>// I²C address 0x77

#include <cmath>// Math functions

#ifndef ALTIMETER
  #define ALTIMETER 1
#endif

#define MS5611_I2C_ADDRESS 0x77// I²C address
#define ALTIMETER_SIZE 2// Sensor data quantity
#define MS5611_PRESSURE_OFFSET -3.37// Pressure offset calibration according to local air pressure (Pa)
#define PRESSURE_KEY "pressao"// JSON pressure key
#define ALTITUDE_KEY "altitude"// JSON altitude key

class Altimeter : public Component{
  private:
    MS5611* baro;
    double altimeter_data[ALTIMETER_SIZE] = {0.f};
    
  public:
    Altimeter();// Create object
    ~Altimeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    double calcAltitude();// Calculates altitude (m)
    void start() override;
};