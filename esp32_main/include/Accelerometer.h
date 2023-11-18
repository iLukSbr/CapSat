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

// MPU-9250 accelerometer/gyroscope
// https://github.com/bolderflight/invensense-imu
#include <mpu6500.h>// I²C address 0x69 (AD0 HIGH to avoid conflict with RTC)

#ifndef ACCELEROMETER
  #define ACCELEROMETER 1
#endif

#define ACCELEROMETER_SIZE 6// Sensor data quantity
#define ACCELEROMETER_SAMPLE_RATE_DIVIDER 19// MPU-9250 sample rate divider
#define ACCELEROMETER_KEY "acelerometro"
#define GYROSCOPE_KEY "giroscopio"

class Accelerometer : public Component{
  private:
    bfs::Mpu6500* imu;
    float accelerometer_data[ACCELEROMETER_SIZE] = {0.f};

  public:
    Accelerometer();// Create object
    ~Accelerometer();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    void start() override;
};