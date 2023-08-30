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

// KY-013 NTC temperature sensor
// https://github.com/YuriiSalimov/NTC_Thermistor
#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>

/*
ESP: VIN = 3.3 V
Analog output = 0 - 3.3 V

Arduino: VIN = 5 V
Analog output = 0 - 5 V
*/

#define NTC_SENSOR_PIN 10// NTC sensor analog pin
#define NTC_REFERENCE_RESISTANCE 9870
#define NTC_NOMINAL_RESISTANCE 10000
#define NTC_NOMINAL_TEMPERATURE 25
#define NTC_B_VALUE 3950
#define NTC_SMOOTHING_FACTOR 5
#define TEMPERATURE_KEY "temperatura"// JSON temperature key

class Thermometer : public Component{
  private:
    Thermistor* originThermistor;
    Thermistor* thermistor;
    float thermometer_data = 0.f;
    
  public:
    Thermometer();// Create object
    ~Thermometer();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    const float getTemperature() const;// Return temperature (°C)
};