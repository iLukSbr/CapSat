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

// QMC5883L magnetometer
// https://github.com/mprograms/QMC5883LCompass
#include <QMC5883LCompass.h>// I²C address 0x0D

#define MAGNETOMETER_KEY "magnetometro"// JSON magnetometer key
#define MAGNETOMETER_DIRECTION_SIZE 4// Direction string length
#define MAGNETOMETER_SIZE 5// Sensor data quantity
#define MAGNETOMETER_I2C_ADDRESS 0x0D// I²C address
#define MAGNETOMETER_SMOOTHING_STEPS 10// Smoothing steps
#define MAGNETOMETER_ADVANCED_SMOOTHING true// Discard extreme values
#define MAGNETOMETER_OSR 0x00// Over sample ratio 512
#define MAGNETOMETER_RNG 0x10// Full scale 8G
#define MAGNETOMETER_ODR 0x0C// Output data rate 200 Hz
#define MAGNETOMETER_MODE 0x01// Continuous mode control
#define MAGNETOMETER_AZIMUTH_KEY "azimute"// JSON compass azimuth key
#define MAGNETOMETER_BEARING_KEY "rumo"// JSON compass bearing key
#define MAGNETOMETER_DIRECTION_KEY "direcao"// JSON compass direction key

class Magnetometer : public Component{
  private:
    QMC5883LCompass* compass;
    int magnetometer_data[MAGNETOMETER_SIZE] = {0};
    char magnetometer_direction[MAGNETOMETER_DIRECTION_SIZE] = {0};

  public:
    Magnetometer();// Create object
    ~Magnetometer();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};