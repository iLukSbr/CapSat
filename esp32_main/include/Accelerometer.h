#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// MPU-9250 accelerometer/gyroscope
// https://github.com/bolderflight/invensense-imu
#include <mpu6500.h>// Address 0x68

#define ACCELEROMETER_SIZE 6// Sensor data quantity
#define ACCELEROMETER_CALIBRATION_DELAY 1000// Delay between calibration requests (ms)
#define ACCELEROMETER_SAMPLE_RATE_DIVIDER 19// MPU-9250 sample rate divider
#define ACCELEROMETER_CALIBRATION_DELAY 1000// Delay between calibration requests (ms)

class Accelerometer : public Component{
  private:
    bfs::Mpu6500* imu = nullptr;
  public:
    float accelerometer_data[ACCELEROMETER_SIZE] = {0};
    void begin() override{
      imu = new bfs::Mpu6500();
      imu->Config(&Wire, bfs::Mpu6500::I2C_ADDR_PRIM);// I²C address 0x68
      while(!imu->Begin()){// Waiting for sensor communication
        delay(ACCELEROMETER_CALIBRATION_DELAY);
        Serial.println(F("Waiting for accelerometer..."));
      }
      while(!imu->ConfigSrd(ACCELEROMETER_SAMPLE_RATE_DIVIDER)){// Set the sample rate divider
        delay(ACCELEROMETER_CALIBRATION_DELAY);// Calibrate moving throug an 8 pattern on a flat surface
        Serial.println(F("Waiting for gyroscope..."));
      }
    }
    void gatherData() override{
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
    void printData() override{// Display data for test
      Serial.print(F("Accelerometer/gyroscope: "));
      for(uint8_t i=0; i<ACCELEROMETER_SIZE; i++){
        Serial.print(accelerometer_data[i]);
        Serial.print(F(" "));
      }
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      for(uint8_t i=0; i<ACCELEROMETER_SIZE; i++){
        my_file->print(accelerometer_data[i]);
        my_file->print(F(","));
      }
    }
};