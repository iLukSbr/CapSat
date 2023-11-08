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

// ENS160+AHT21 carbodioximeter, TVOC (total volatile organic compounds) meter and humidimeter
// https://github.com/DFRobot/DFRobot_ENS160
// https://github.com/enjoyneering/AHTxx
#include <DFRobot_ENS160.h>// I²C address 0x53
#include <AHTxx.h>// I²C address 0x38

#ifndef HUMIDIMETER
  #define HUMIDIMETER 1
#endif

#define HUMIDIMETER_SIZE 5// Sensor data quantity
#define HUMIDITY_KEY "umidade"// JSON humidity key
#define TEMPERATURE_KEY "temperatura"// JSON temperature key
#define TVOC_KEY "TCOV"// JSON Total Volatile Organic Compounds (TVOC) key
#define CO2_KEY "CO2"// JSON CO2 key
#define AQI_KEY "IQA"// JSON Air Quality Index (AQI) key

class Humidimeter : public Component{
  private:
    AHTxx* aht21;
    DFRobot_ENS160_I2C* ens160;
    float humidimeter_data[HUMIDIMETER_SIZE] = {0.f};
    
  public:
    Humidimeter();// Create object
    ~Humidimeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    const float getHumidity() const;// Return humidity (%)
    const float getTemperature() const;// Return temperature (°C)
    void start() override;
};