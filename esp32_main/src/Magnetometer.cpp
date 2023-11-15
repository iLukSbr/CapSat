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
    compass(new QMC5883LCompass())// Instantiate compass
{// Create object
    multiPrintln(F("Starting magnetometer QMC5883L..."));
    compass->setADDR(MAGNETOMETER_I2C_ADDRESS);
    compass->init();
    compass->setMode(MAGNETOMETER_MODE, MAGNETOMETER_ODR, MAGNETOMETER_RNG, MAGNETOMETER_OSR);
    compass->setSmoothing(MAGNETOMETER_SMOOTHING_STEPS, MAGNETOMETER_ADVANCED_SMOOTHING);
    // compass->setCalibrationOffsets(-790, 1387, -2097, 601, -695, 2153);
    start();
}

Magnetometer::~Magnetometer(){// Release memory
    delete compass;
}

void Magnetometer::calibrate(){// Calibrate offsets
    float x=0.0, y=0.0, z=0.0;
    multiPrintln(F("Calibrating magnetometer QMC5883L..."));
    for(byte i=0; i<50; i++){
        compass->read();
        delay(500);
        x += (float)compass->getX();
        y += (float)compass->getY();
        z += (float)compass->getZ();        
    }
    compass->setCalibrationOffsets(-x/256, -y/256, -z/256);
}

void Magnetometer::gatherData(){// Get data from component
    multiPrintln(F("Gathering magnetometer QMC5883L data..."));
    compass->read();
    delay(1000);
	magnetometer_data[0] = compass->getX();// X
	magnetometer_data[1] = compass->getY();// Y
	magnetometer_data[2] = compass->getZ();// Z
	magnetometer_data[3] = compass->getAzimuth();// Azimuth
	magnetometer_data[4] = compass->getBearing(magnetometer_data[3]);// Bearing
    compass->getDirection(magnetometer_direction, magnetometer_data[3]);// Direction
    magnetometer_direction[3] = '\0';
}

void Magnetometer::printData(){// Display data for test
    multiPrint(F("Magnetometer QMC5883L: "));
    for(uint8_t i=0; i<MAGNETOMETER_SIZE; i++){
        multiPrint(magnetometer_data[i]);
        multiPrint(F(" "));
    }
    multiPrintln(magnetometer_direction);
}

void Magnetometer::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    if(!isHTTP){
        for(uint8_t i=0; i<3; i++)
            payload[F(MAGNETOMETER_KEY)][i] = magnetometer_data[i];
        payload[F(MAGNETOMETER_AZIMUTH_KEY)] = magnetometer_data[3];
        payload[F(MAGNETOMETER_BEARING_KEY)] = magnetometer_data[4];
    }
    payload[F(MAGNETOMETER_DIRECTION_KEY)] = magnetometer_direction;
}

void Magnetometer::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<MAGNETOMETER_SIZE; i++){
        my_file->print(magnetometer_data[i]);
        my_file->print(F(","));
    }
    my_file->print(magnetometer_direction);
    my_file->print(F(","));
}

void Magnetometer::start(){
    calibrate();
    multiPrintln(F("Magnetometer QMC5883L OK!"));
    started = true;
}