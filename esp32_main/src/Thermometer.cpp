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

#include "pch.h"
#include "Thermometer.h"

Thermometer::Thermometer():
    originThermistor(new NTC_Thermistor(NTC_SENSOR_PIN, NTC_REFERENCE_RESISTANCE, NTC_NOMINAL_RESISTANCE, NTC_NOMINAL_TEMPERATURE, NTC_B_VALUE)),
    thermistor(new SmoothThermistor(originThermistor, NTC_SMOOTHING_FACTOR))
{// Create object

}

Thermometer::~Thermometer(){// Release memory
    delete originThermistor;
    delete thermistor;
}

void Thermometer::gatherData(){// Get data from component
    thermometer_data = thermistor->readCelsius();// Analog voltage reading and conversion to temperature in Celsius degree (°)
}

void Thermometer::printData(){// Display data for test
    Serial.print(F("Termômetro: "));
    Serial.print(thermometer_data);
    Serial.println();
}

void Thermometer::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    doc[F(TEMPERATURE_KEY)] = thermometer_data;
}

void Thermometer::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(thermometer_data);
    my_file->print(F(","));
}

const float Thermometer::getTemperature() const{// Return temperature (°C)
    return thermometer_data;
}