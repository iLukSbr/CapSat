#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// MS5611 altimeter/barometer
// https://github.com/RobTillaart/MS5611
#include <MS5611.h>// Address 0x77

#define ALTIMETER_SIZE 2// Sensor data quantity
#define MS5611_PRESSURE_OFFSET -450// Pressure offset calibration according to local air pressure (Pa)
#define PRESSURE_KEY "pressao"// JSON pressure key
#define ALTITUDE_KEY "altitude"// JSON altitude key

class Altimeter : public Component{
  private:
    MS5611* baro = nullptr;
    float altimeter_data[ALTIMETER_SIZE] = {0.f};
  public:
    Altimeter(){
      baro = new MS5611();// Instantiate sensor
      baro->begin();// Start sensor
      baro->pressureOffset = MS5611_PRESSURE_OFFSET;// Calibrate according to local air pressure (Pa)
    }
    ~Altimeter(){
      delete baro;
    }
    void gatherData() override{
      altimeter_data[0] = baro->getPressure();// Air pressure (Pa)
      altimeter_data[1] = baro->getAltitude();// Altitude (m)
    }
    void printData() override{// Display data for test
      Serial.print(F("Altimeter/barometer: "));
      for(uint8_t i=0; i<ALTIMETER_SIZE; i++){
        Serial.print(altimeter_data[i]);
        Serial.print(F(" "));
      }
      Serial.println();
    }
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
        doc[F(PRESSURE_KEY)] = altimeter_data[0];
        if(!isHTTP)
          payload[F(ALTITUDE_KEY)] = altimeter_data[1];
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
      for(uint8_t i=0; i<ALTIMETER_SIZE; i++){
        my_file->print(altimeter_data[i]);
        my_file->print(F(","));
      }
    }
};