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
#include "Multimeter.h"

Multimeter::Multimeter():
    mult(new INA219_WE(0x40))// I²C address
{// Create object
    multiPrintln(F("Starting multimeter..."));
    while(!mult->init()){// Calibrating
        delay(1000);
        multiPrintln(F("Aguardando o voltímetro."));
    }
    mult->setADCMode(SAMPLE_MODE_128);// Mean value of 128 measures
    mult->setMeasureMode(CONTINUOUS);// Continuous data transmission
    mult->setPGain(PG_40);// Max shunt voltage 40 mV
    mult->setShuntSizeInOhms(SHUNT_RESISTANCE);// Shunt resistance
    multiPrintln(F("Multimeter OK!"));
}

Multimeter::~Multimeter(){// Release memory
    delete mult;
}

void Multimeter::gatherData(){// Get data from component
    multiPrintln(F("Gathering multimeter data..."));
    multimeter_data[0] = (float)map(mult->readRegister(0x02),2,866,2400,15740)/1000.f;// Voltage (V)
    multiPrint("Raw voltage reading: ");
    multiPrintln(mult->readRegister(0x02));
    multimeter_data[1] = constrain((float)map((long)(multimeter_data[0]*100.0), 1000, 1680, 0, 100000)/1000.f, 0.f, 100.f);// Battery percentage (%)
    multimeter_data[2] = mult->getCurrent_mA();// Electric current (mA)
    multimeter_data[3] = multimeter_data[2]*multimeter_data[0];// Electric power (mW)
}

void Multimeter::printData(){// Display data for test
    multiPrint(F("Multimeter: "));
    for(uint8_t i=0; i<2; i++){
        multiPrint(multimeter_data[i]);
        multiPrint(F(" "));
    }
    multiPrintln();
}

void Multimeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    doc[F(BATTERY_KEY)] = round(multimeter_data[1]);
    if(!isHTTP){
        payload[F(VOLTAGE_KEY)] = multimeter_data[0];
        payload[F(CURRENT_KEY)] = multimeter_data[2];
        payload[F(POWER_KEY)] = multimeter_data[3];
    }
}

void Multimeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<2; i++){
        my_file->print(multimeter_data[i]);
        my_file->print(F(","));
    }
}