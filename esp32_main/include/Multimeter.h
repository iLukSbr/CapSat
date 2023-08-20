#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// INA219 multimeter
// https://github.com/wollewald/INA219_WE
#include <INA219_WE.h>// Address 0x40

#define MULTIMETER_SIZE 3// Sensor data quantity
#define SHUNT_RESISTANCE 0.01// Shunt resistance 10R = 0.01K
#define BATTERY_KEY "bateria"// JSON battery key
#define VOLTAGE_KEY "tensao"// JSON voltage key
#define CURRENT_KEY "corrente"// JSON electric current key
#define POWER_KEY "potencia"// JSON electric power key

class Multimeter : public Component{
  private:
    INA219_WE* mult = nullptr;
    float multimeter_data[MULTIMETER_SIZE] = {0.f};
  public:
    Multimeter(){
      mult = new INA219_WE(0x40);// I²C address
      while(!mult->init()){// Calibrating
        delay(1000);
        Serial.println(F("Aguardando o voltímetro."));
      }
      mult->setADCMode(SAMPLE_MODE_128);// Mean value of 128 measures
      mult->setMeasureMode(CONTINUOUS);// Continuous data transmission
      mult->setPGain(PG_40);// Max shunt voltage 40 mV
      mult->setShuntSizeInOhms(SHUNT_RESISTANCE);// Shunt resistance
    }
    ~Multimeter(){
      delete mult;
    }
    void gatherData() override{
      multimeter_data[0] = (float)map(mult->readRegister(0x02),2,610,234,1253)/100.0;// Voltage (V)
      multimeter_data[1] = constrain((float)map((long)(multimeter_data[0]*100.0), 1000, 1680, 0, 10000)/100.0, 0.0, 100.0);// Battery percentage (%)
      multimeter_data[2] = mult->getCurrent_mA();// Electric current (mA)
      multimeter_data[3] = multimeter_data[2]*multimeter_data[0];// Electric power (mW)
    }
    void printData() override{// Display data for test
      for(uint8_t i=0; i<2; i++){
        Serial.print(multimeter_data[i]);
        Serial.print(F(" "));
      }
      Serial.println();
    }
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
      doc[F(BATTERY_KEY)] = multimeter_data[1];
      if(!isHTTP){
        payload[F(VOLTAGE_KEY)] = multimeter_data[0];
        payload[F(CURRENT_KEY)] = multimeter_data[2];
        payload[F(POWER_KEY)] = multimeter_data[3];
      }
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
      for(uint8_t i=0; i<2; i++){
        my_file->print(multimeter_data[i]);
        my_file->print(F(","));
      }
    }
};