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
#include "ParticulateMeter.h"

ParticulateMeter::ParticulateMeter():
    #if defined(ESP32) || defined(ESP8266)// For ESP
        pmSerial(new HardwareSerial(UART_NUM_2)),
    #else// For Arduino
        pmSerial(new SoftwareSerial(PM_RX_PIN, PM_TX_PIN)),
    #endif
    aqi(new Adafruit_PM25AQI())
{// Create object
    multiPrintln(F("Starting particulate meter..."));
    pmSerial->begin(SERIAL_BAUD_RATE);
    while(!aqi->begin_UART(pmSerial)){
        delay(CALIBRATION_DELAY);
        multiPrintln(F("Waiting for particulate meter..."));
    }
    multiPrintln(F("Particulate meter OK!"));
}

ParticulateMeter::~ParticulateMeter(){// Release memory
    delete pmSerial;
    delete aqi;
}

void ParticulateMeter::gatherData(){// Get data from component
    multiPrintln(F("Gathering particulate meter data..."));
    PM25_AQI_Data data;
    if(aqi->read(&data)){
        // "Less than" particle concentrations at sea level pressure
        particulate_meter_data[0] = data.pm10_standard;// PM1 (<1 micrometer diameter)
        particulate_meter_data[1] = data.pm25_standard;// PM2.5 (<2,5 micrometer diameter)
        particulate_meter_data[2] = data.pm100_standard;// PM10 (<10 micrometer diameter)

        // "Less than" particle concentrations at local pressure
        particulate_meter_data[3] = data.pm10_env;// PM1 (<1 micrometer diameter)
        particulate_meter_data[4] = data.pm25_env;// PM2.5 (<2,5 micrometer diameter)
        particulate_meter_data[5] = data.pm100_env;// PM10 (<10 micrometer diameter)

        // "Greater than" particle concentrations at local pressure
        particulate_meter_data[6] = data.particles_03um;// > 0.3um / 0.1L air
        particulate_meter_data[7] = data.particles_05um;// > 0.5um / 0.1L air
        particulate_meter_data[8] = data.particles_10um;// > 1.0um / 0.1L air
        particulate_meter_data[9] = data.particles_25um;// > 2.5um / 0.1L air
        particulate_meter_data[10] = data.particles_50um;// > 5.0um / 0.1L air
        particulate_meter_data[11] = data.particles_100um;// > 10 um / 0.1L air
    }
}

void ParticulateMeter::printData(){// Display data for test
    multiPrint(F("Particulate meter: "));
    for(uint8_t i=0; i<PM_SIZE; i++){
        multiPrint(particulate_meter_data[i]);
        multiPrint(F(" "));
    }
    multiPrintln();
}

void ParticulateMeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(PML1L_KEY)] = particulate_meter_data[3];// PM1 (<1 micrometer diameter)
    payload[F(PML2_5L_KEY)] = particulate_meter_data[4];// PM2.5 (<2,5 micrometer diameter)
    payload[F(PML10L_KEY)] = particulate_meter_data[5];// PM10 (<10 micrometer diameter)
    payload[F(PMG0_3_KEY)] = particulate_meter_data[6];// > 0.3um / 0.1L air
    payload[F(PMG0_5_KEY)] = particulate_meter_data[7];// > 0.5um / 0.1L air
    payload[F(PMG1_KEY)] = particulate_meter_data[8];// > 1.0um / 0.1L air
    payload[F(PMG2_5_KEY)] = particulate_meter_data[9];// > 2.5um / 0.1L air
    payload[F(PMG5_KEY)] = particulate_meter_data[10];// > 5.0um / 0.1L air
    payload[F(PMG10_KEY)] = particulate_meter_data[11];// > 10 um / 0.1L air
    if(!isHTTP){
        payload[F(PML1S_KEY)] = particulate_meter_data[0] ;// PM1 (<1 micrometer diameter)
        payload[F(PML2_5S_KEY)] = particulate_meter_data[1];// PM2.5 (<2,5 micrometer diameter)
        payload[F(PML10S_KEY)] = particulate_meter_data[2];// PM10 (<10 micrometer diameter)
    }
}

void ParticulateMeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<PM_SIZE; i++){
        my_file->print(particulate_meter_data[i]);
        my_file->print(F(","));
    }
}