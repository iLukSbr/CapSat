#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// QMC5883L magnetometer
// https://github.com/dthain/QMC5883L
#include <QMC5883L.h>// Address 0x0D

class Magnetometer : public Component{
  private:
    QMC5883L* compass = nullptr;
  public:
    int magnetometer_data = 0;
    void begin() override{
      compass = new QMC5883L();
      compass->init();// Calibrar movendo em forma de 8 sobre superfície plana
      compass->setSamplingRate(50);// Hz
    }
    void gatherData() override{
      magnetometer_data = compass->readHeading();// Lendo a bússola
    }
    void printData() override{// Display data for test
      Serial.print(F("Magnetômetro: "));
      Serial.print(magnetometer_data);
      Serial.println();
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(magnetometer_data);
      my_file->print(F(","));
    }
};