#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// QMC5883L magnetometer
// https://github.com/dthain/QMC5883L
#include <QMC5883L.h>// Address 0x0D

#define MAGNETOMETER_KEY "magnetometro"// JSON magnetometer key

class Magnetometer : public Component{
  private:
    QMC5883L* compass = nullptr;
    int magnetometer_data = 0;

  public:
    Magnetometer();// Create object
    ~Magnetometer();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};