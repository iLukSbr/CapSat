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
#include "Altimeter.h"

Altimeter::Altimeter():
    baro(new MS5611())// Instantiate sensor
{// Create object
    multiPrintln(F("Starting altimeter..."));
    baro->begin();// Start sensor
    baro->setPressureOffset(MS5611_PRESSURE_OFFSET);// Calibrate according to local air pressure (Pa)
    multiPrintln(F("Altimeter OK!"));
}

Altimeter::~Altimeter(){// Release memory
    delete baro;
}

void Altimeter::gatherData(){// Get data from component
    multiPrintln(F("Gathering altimeter data..."));
    altimeter_data[0] = baro->getPressure();// Air pressure (Pa)
    altimeter_data[1] = baro->getAltitude();// Altitude (m)
}

void Altimeter::printData(){// Display data for test
    multiPrint(F("Altimeter/barometer: "));
    for(uint8_t i=0; i<ALTIMETER_SIZE; i++){
        multiPrint(altimeter_data[i]);
        multiPrint(F(" "));
    }
    multiPrintln();
}

void Altimeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    doc[F(PRESSURE_KEY)] = altimeter_data[0];
    if(!isHTTP)
        payload[F(ALTITUDE_KEY)] = altimeter_data[1];
}

void Altimeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<ALTIMETER_SIZE; i++){
        my_file->print(altimeter_data[i]);
        my_file->print(F(","));
    }
}