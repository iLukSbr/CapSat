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
#include "Humidimeter.h"

Humidimeter::Humidimeter():
    aht21(new AHTxx(AHTXX_ADDRESS_X38, AHT2x_SENSOR)),// Instantiate humidity/temperature sensor
    ens160(new DFRobot_ENS160_I2C(&Wire, 0x53))// Instantiate gas sensor I²C addres 0x53
{// Create object
    start();
}

Humidimeter::~Humidimeter(){// Release memory
    delete aht21;
    delete ens160;
}

void Humidimeter::gatherData(){// Get data from component
    multiPrintln(F("Gathering humidimeter/thermometer/TVOC/CO2/AQI sensor ENS160+AHT21 data..."));
    humidimeter_data[0] = aht21->readTemperature();// Get temperature (°C)
    humidimeter_data[1] = aht21->readHumidity();// Get humidity (%)
    ens160->setTempAndHum(humidimeter_data[0], humidimeter_data[1]);// Calibrate gas sensor using temperature and humidity
    humidimeter_data[2] = ens160->getTVOC();// Concentration of total volatile organic compounds (ppb)
    humidimeter_data[3] = ens160->getECO2();// Carbon dioxide equivalent concentration (ppm)
    humidimeter_data[4] = ens160->getAQI();// Air quality index
}

void Humidimeter::printData(){// Display data for test
    multiPrint(F("Humidimeter/thermometer/TVOC/CO2/AQI sensor ENS160+AHT21: "));
    for(uint8_t i=0; i<HUMIDIMETER_SIZE; i++){
        multiPrint(humidimeter_data[i]);
        multiPrint(F(" "));
    }
    multiPrintln();
}

void Humidimeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    doc[F(TEMPERATURE_KEY)] = humidimeter_data[0];
    payload[F(HUMIDITY_KEY)] = humidimeter_data[1];
    payload[F(TVOC_KEY)] = humidimeter_data[2];
    payload[F(CO2_KEY)] = humidimeter_data[3];
    if(!isHTTP)
        payload[F(AQI_KEY)] = humidimeter_data[4];
}

void Humidimeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<HUMIDIMETER_SIZE; i++){
        my_file->print(humidimeter_data[i],4);
        my_file->print(F(","));
    }
}

void Humidimeter::start(){
    float ahtTemperature;
    multiPrintln(F("Starting humidimeter/thermometer/TVOC/CO2/AQI sensor ENS160+AHT21..."));
    for(byte i=0; i<START_TRIES; i++){// Calibrate AHT21
        if(aht21->begin()){
            ahtTemperature = aht21->readTemperature();// Get temperature (°C)
            if(ahtTemperature == AHTXX_ERROR)// If temperature error
                ahtTemperature = 0;
            humidimeter_data[0] = aht21->readHumidity();// Get humidity (%)
            if(humidimeter_data[0] == AHTXX_ERROR)// If humidity error
                humidimeter_data[0] = 0;
            for(byte j=0; j<START_TRIES; j++){// Calibrate ENS160
                if(ens160->begin() == NO_ERR){
                    ens160->setPWRMode(ENS160_STANDARD_MODE);// Normal operation
                    started = true;
                    multiPrintln(F("Humidimeter/thermometer/TVOC/CO2/AQI sensor ENS160+AHT21 OK!"));
                    break;
                }
                multiPrintln(F("Waiting for TVOC/CO2/AQI sensor ENS160..."));
                delay(CALIBRATION_DELAY);
            }
            if(isStarted())
                break;
        }
        multiPrintln(F("Waiting for humidimeter/thermometer AHT21..."));
        delay(CALIBRATION_DELAY);
    }
}

const float Humidimeter::getHumidity() const{// Return humidity (%)
    return humidimeter_data[1];
}

const float Humidimeter::getTemperature() const{// Return temperature (°C)
    return humidimeter_data[0];
}