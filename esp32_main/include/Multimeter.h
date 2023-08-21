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

// INA219 multimeter
// https://github.com/wollewald/INA219_WE
#include <INA219_WE.h>// Address 0x40

#define MULTIMETER_SIZE 3// Sensor data quantity
#define SHUNT_RESISTANCE 0.01// Shunt resistance 10R = 0.01K
#define BATTERY_KEY "bateria"// JSON battery key
#define VOLTAGE_KEY "tensao"// JSON voltage key
#define CURRENT_KEY "corrente"// JSON electric current key
#define POWER_KEY "potencia"// JSON electric power key

class Multimeter : public Component{
  private:
    INA219_WE* mult = nullptr;
    float multimeter_data[MULTIMETER_SIZE] = {0.f};
    
  public:
    Multimeter();// Create object
    ~Multimeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};