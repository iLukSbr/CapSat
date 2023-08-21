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
#include "Accelerometer.h"

Accelerometer::Accelerometer(){// Create object
    imu = new bfs::Mpu6500();
    imu->Config(&Wire, bfs::Mpu6500::I2C_ADDR_PRIM);// I²C address 0x68
    while(!imu->Begin()){// Waiting for sensor communication
        delay(CALIBRATION_DELAY);
        Serial.println(F("Waiting for accelerometer..."));
    }
    while(!imu->ConfigSrd(ACCELEROMETER_SAMPLE_RATE_DIVIDER)){// Set the sample rate divider
        delay(CALIBRATION_DELAY);// Calibrate moving throug an 8 pattern on a flat surface
        Serial.println(F("Waiting for gyroscope..."));
    }
}

Accelerometer::~Accelerometer(){
    delete imu;
}

void Accelerometer::gatherData(){// Get data from component
    if(imu->Read()){// If something was received
        // Accelerometer
        accelerometer_data[0] = imu->accel_x_mps2();// X axis
        accelerometer_data[1] = imu->accel_y_mps2();// Y axis
        accelerometer_data[2] = imu->accel_z_mps2();// Z axis

        // Gyroscope
        accelerometer_data[3] = imu->gyro_x_radps();// X axis
        accelerometer_data[4] = imu->gyro_y_radps();// Y axis
        accelerometer_data[5] = imu->gyro_z_radps();// Z axis
    }
}

void Accelerometer::printData(){// Display data for test
    Serial.print(F("Accelerometer/gyroscope: "));
    for(uint8_t i=0; i<ACCELEROMETER_SIZE; i++){
        Serial.print(accelerometer_data[i]);
        Serial.print(F(" "));
    }
}

void Accelerometer::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    uint8_t i, j;
    for(i=0, j=0; i<3 && j<3; i++, j++)
        doc[ACCELEROMETER_KEY][j] = accelerometer_data[i];
    for(i=3, j=0; i<6 && j<3; i++, j++)
        doc[GYROSCOPE_KEY][j] = accelerometer_data[i];
}

void Accelerometer::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<ACCELEROMETER_SIZE; i++){
        my_file->print(accelerometer_data[i]);
        my_file->print(F(","));
    }
}