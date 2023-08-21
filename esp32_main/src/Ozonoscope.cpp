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
#include "Ozonoscope.h"

Ozonoscope::Ozonoscope(){// Create object
    MQ131 = new MQUnifiedsensor(OZONOSCOPE_BOARD, PIN_MAX_VOLTAGE, ADC_RESOLUTION, OZONOSCOPE_PIN, OZONOSCOPE_NAME);
    MQ131->setRegressionMethod(OZONOSCOPE_REGRESSION_METHOD);// Set regression method a*ratio^b
    MQ131->setA(OZONOSCOPE_A_PARAMETER);// Configure the a to to calculate O3 concentration
    MQ131->setB(OZONOSCOPE_B_PARAMETER);// Configure the b to to calculate O3 concentration
    MQ131->init();// Start
    float calcR0 = 0.f;
    for(int i = 1; i<=OZONOSCOPE_CALIBRATION_LOOP; i++){
        MQ131->update();// Update data reading the voltage from the analog pin
        calcR0 += MQ131->calibrate(OZONOSCOPE_CLEAN_AIR_RATIO);// Calibrate
    }
    MQ131->setR0(calcR0/OZONOSCOPE_CALIBRATION_LOOP);// Set R0 and calibrate
    while(isinf(calcR0)){
        delay(CALIBRATION_DELAY);
        Serial.println(F("Waiting for ozonoscope..."));
    }
    while(calcR0 == 0){
        delay(CALIBRATION_DELAY);
        Serial.println(F("Waiting for ozonoscope..."));
    }
    MQ131->setOffset(OZONOSCOPE_OFFSET);// Calibration offset
}

Ozonoscope::~Ozonoscope(){// Release memory
    delete MQ131;
}

void Ozonoscope::gatherData(){// Get data from component
    MQ131->update();// Get data
    MQ131->readSensorR0Rs();// Calibrate
    ozonoscope_data = MQ131->getConcentration();// Ozone concentration in air (ppb)
}

void Ozonoscope::printData(){// Display data for test
    Serial.print(F("Ozonoscope: "));
    Serial.print(ozonoscope_data, OZONOSCOPE_DECIMAL_PLACES);
    Serial.println();
}

void Ozonoscope::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(O3_KEY)] = ozonoscope_data;
}

void Ozonoscope::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(ozonoscope_data, OZONOSCOPE_DECIMAL_PLACES);
    my_file->print(F(","));
}