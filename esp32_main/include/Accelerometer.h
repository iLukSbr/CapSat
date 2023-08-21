#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// MPU-9250 accelerometer/gyroscope
// https://github.com/bolderflight/invensense-imu
#include <mpu6500.h>// Address 0x68

#define ACCELEROMETER_SIZE 6// Sensor data quantity
#define ACCELEROMETER_SAMPLE_RATE_DIVIDER 19// MPU-9250 sample rate divider

class Accelerometer : public Component{
  private:
    bfs::Mpu6500* imu = nullptr;
    const String ACCELEROMETER_KEY = F("acelerometro");// JSON accelerometer key
    const String GYROSCOPE_KEY = F("giroscopio");// JSON gyroscope key
    float accelerometer_data[ACCELEROMETER_SIZE] = {0.f};

  public:
    Accelerometer();// Create object
    ~Accelerometer();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};