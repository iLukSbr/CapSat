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
    Magnetometer(){
      compass = new QMC5883L();// Instantiate compass
      compass->init();// Calibrate moving throug an 8 pattern on a flat surface
      compass->setSamplingRate(50);// Hz
    }
    ~Magnetometer(){
      delete compass;
    }
    void gatherData() override{
      magnetometer_data = compass->readHeading();// Compass direction (°)
    }
    void printData() override{// Display data for test
      Serial.print(F("Magnetômetro: "));
      Serial.print(magnetometer_data);
      Serial.println();
    }
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
      if(!isHTTP)
        payload[F(MAGNETOMETER_KEY)] = magnetometer_data;
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(magnetometer_data);
      my_file->print(F(","));
    }
};