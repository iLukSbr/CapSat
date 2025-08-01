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

#include <cmath>// Math functions

#include "settings.h"
#include "Ozonoscope.h"

Ozonoscope::Ozonoscope():
    adc(new ADS1115_WE(ADC_I2C_ADDRESS))
{// Create object
    MQ131.begin(OZONOSCOPE_HEATER_PIN, ADS1115_COMP_0_GND, LOW_CONCENTRATION, OZONOSCOPE_RL, OZONOSCOPE_CALIBRATION_CYCLE, adc); 
    multiPrintln(F("Calibrating ozonoscope MQ-131-Low..."));
    MQ131.calibrate();// Calibrate
    start();
}

Ozonoscope::~Ozonoscope(){// Release memory
    delete adc;
}

void Ozonoscope::gatherData(){// Get data from component
    multiPrintln(F("Gathering ozonoscope MQ-131-Low data..."));
    MQ131.sample();// Get data
    ozonoscope_data = MQ131.getO3(PPB);// Ozone concentration in air (ppb)
}

void Ozonoscope::printData(){// Display data for test
    multiPrint(F("Ozonoscope MQ-131-Low: "));
    multiPrint(ozonoscope_data);
    multiPrintln();
}

void Ozonoscope::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(O3_KEY)] = ozonoscope_data;
}

void Ozonoscope::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(ozonoscope_data);
    my_file->print(F(","));
}

void Ozonoscope::setClimateParameters(float temperature, float humidity){// Define climate parameters
    MQ131.setEnv((int8_t)round(temperature), (uint8_t)round(humidity));
}

void Ozonoscope::start(){
    multiPrintln(F("Starting ozonoscope MQ-131-Low..."));
    for(byte i=0; i<START_TRIES; i++){
        if(adc->init()){
            started = true;
            multiPrintln(F("Ozonoscope MQ-131-Low OK!"));
            break;
        }
        // multiPrintln(F("Waiting for ozonoscope ADC ADS1115..."));
        delay(CALIBRATION_DELAY);
    }
}