#pragma once

#include "Component.h"

// MQ-131 ozonoscope
// https://github.com/miguel5612/MQSensorsLib
#include <MQUnifiedsensor.h>

#define OZONOSCOPE_PIN A0// Ozonoscope analog pin
#define OZONOSCOPE_A_PARAMETER 23.943// Configure the equation to to calculate O3 concentration
#define OZONOSCOPE_B_PARAMETER -1.11// Configure the equation to to calculate O3 concentration
#define OZONOSCOPE_REGRESSION_METHOD 1// Set regression method a*ratio^b
#define OZONOSCOPE_OFFSET 0.1// Value offset
#define OZONOSCOPE_CLEAN_AIR_RATIO 15// Clean air ratio RS/R0 = 15 ppm
#define OZONOSCOPE_CALIBRATION_LOOP 10// How many times MQ-131 will calibrate
#define OZONOSCOPE_VOLTAGE_RESOLUTION 5// MQ-131 voltage resolution
#define OZONOSCOPE_ADC_BIT_RESOLUTION 10// MQ-131 ADC bit resolution
#define OZONOSCOPE_DECIMAL_PLACES 4// Concentration decimal places
#define OZONOSCOPE_NAME "MQ-131"// MQ-131 sensor name

#ifdef ESP32
  #define OZONOSCOPE_BOARD "ESP32"// For ESP32
#elif defined(ESP8266)
  #define OZONOSCOPE_BOARD "ESP8266"// For ESP8266
#else
  #define OZONOSCOPE_BOARD "Arduino"// For Arduino
#endif

class Ozonoscope : public Component{
  private:
    MQUnifiedsensor* MQ131 = nullptr;
  public:
    float ozonoscope_data;
    void begin() override{
      MQ131 = new MQUnifiedsensor(OZONOSCOPE_BOARD, OZONOSCOPE_VOLTAGE_RESOLUTION, OZONOSCOPE_ADC_BIT_RESOLUTION, OZONOSCOPE_PIN, OZONOSCOPE_NAME);
      MQ131->setRegressionMethod(OZONOSCOPE_REGRESSION_METHOD);// Set regression method a*ratio^b
      MQ131->setA(OZONOSCOPE_A_PARAMETER);// Configure the a to to calculate O3 concentration
      MQ131->setB(OZONOSCOPE_B_PARAMETER);// Configure the b to to calculate O3 concentration
      MQ131->init();// Start
      float calcR0 = 0;
      for(int i = 1; i<=OZONOSCOPE_CALIBRATION_LOOP; i++)
      {
        MQ131->update();// Update data reading the voltage from the analog pin
        calcR0 += MQ131->calibrate(OZONOSCOPE_CLEAN_AIR_RATIO);// Calibrate
      }
      MQ131->setR0(calcR0/OZONOSCOPE_CALIBRATION_LOOP);// Set R0 and calibrate
      while(isinf(calcR0)){
        delay(1000);
        Serial.println(F("Waiting for ozonoscope..."));
      }
      while(calcR0 == 0){
        delay(1000);
        Serial.println(F("Waiting for ozonoscope..."));
      }
      MQ131->serialDebug(true);
      MQ131->setOffset(OZONOSCOPE_OFFSET);
    }
    void gatherData() override{
      MQ131->update();// Get data
      MQ131->readSensorR0Rs();// Calibrate
      ozonoscope_data = MQ131->getConcentration();// Ozone concentration in ppb
    }
    void printData() override{// Display data for test
      Serial.print(F("Ozonoscope: "));
      Serial.print(ozonoscope_data, OZONOSCOPE_DECIMAL_PLACES);
      Serial.println();
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(ozonoscope_data, OZONOSCOPE_DECIMAL_PLACES);
      my_file->print(F(","));
    }
};