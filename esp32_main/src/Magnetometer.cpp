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
#include "Magnetometer.h"

Magnetometer::Magnetometer():
    magnetometer_data(0),
    compass(new QMC5883L())// Instantiate compass
{// Create object
    compass->init();// Calibrate moving throug an 8 pattern on a flat surface
    compass->setSamplingRate(50);// Amostras (Hz)
}

Magnetometer::~Magnetometer(){// Release memory
    delete compass;
}

void Magnetometer::gatherData(){// Get data from component
    magnetometer_data = compass->readHeading();// Compass direction (°)
}

void Magnetometer::printData(){// Display data for test
    Serial.print(F("Magnetômetro: "));
    Serial.print(magnetometer_data);
    Serial.println();
}

void Magnetometer::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    if(!isHTTP)
        payload[F(MAGNETOMETER_KEY)] = magnetometer_data;
}

void Magnetometer::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(magnetometer_data);
    my_file->print(F(","));
}