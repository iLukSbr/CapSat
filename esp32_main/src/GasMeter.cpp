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
#include "GasMeter.h"

GasMeter::GasMeter(){// Create object
    gas = new MICS6814(GAS_METER_CO_PIN, GAS_METER_NO2_PIN, GAS_METER_NH3_PIN);
    gas->calibrate();
}

GasMeter::~GasMeter(){// Release memory
    delete gas;
}

void GasMeter::gatherData(){// Get data from component
    gas_meter_data[0] = gas->measure(NH3);// Ammonia concentration in air (ppm)
    gas_meter_data[1] = gas->measure(CO);// Carbon monoxide concentration in air (ppm)
    gas_meter_data[2] = gas->measure(NO2);// Nitrogen dioxide concentration in air (ppm)
    gas_meter_data[3] = gas->measure(C3H8);// Propane concentration in air (ppm)
    gas_meter_data[4] = gas->measure(C4H10);// Butane concentration in air (ppm)
    gas_meter_data[5] = gas->measure(CH4);// Methane concentration in air (ppm)
    gas_meter_data[6] = gas->measure(H2);// Hydrogen gas concentration in air (ppm)
    gas_meter_data[7] = gas->measure(C2H5OH);// Ethanol concentration in air (ppm)
}

void GasMeter::printData(){// Display data for test
    Serial.print(F("Gas meter: "));
    for(uint8_t i=0; i<GAS_METER_SIZE; i++){
        Serial.print(gas_meter_data[i], GAS_DECIMAL_PLACES);
        Serial.print(F(" "));
    }
    Serial.println();
}

void GasMeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(NH3_KEY)] = gas_meter_data[0];// Ammonia
    payload[F(CO_KEY)] = gas_meter_data[1];// Carbon monoxide
    payload[F(NO2_KEY)] = gas_meter_data[2];// Nitrogen dioxide
    payload[F(C3H8_KEY)] = gas_meter_data[3];// Propane
    payload[F(C4H10_KEY)] = gas_meter_data[4];// Butane
    payload[F(CH4_KEY)] = gas_meter_data[5];// Methane
    payload[F(H2_KEY)] = gas_meter_data[6];// Hydrogen gas
    payload[F(C2H5OH_KEY)] = gas_meter_data[7];// Ethanol
}

void GasMeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<GAS_METER_SIZE; i++){
        my_file->print(gas_meter_data[i], GAS_DECIMAL_PLACES);
        my_file->print(F(","));
    }
}