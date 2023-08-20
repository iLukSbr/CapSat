#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// ENS160+AHT21 carbodioximeter, TVOC (total volatile organic compounds) meter and humidimeter
// https://github.com/DFRobot/DFRobot_ENS160
// https://github.com/enjoyneering/AHTxx
#include <DFRobot_ENS160.h>// Address 0x53
#include <AHTxx.h>// Address 0x38

class Humidimeter : public Component{
  private:
    AHTxx* aht21 = nullptr;
    DFRobot_ENS160_I2C* ENS160 = nullptr;
    float ahtTemperature;
    float ahtHumidity;
    void printStatus(){
      switch(aht21->getStatus()){
        case AHTXX_NO_ERROR:
          Serial.println(F("no error"));
          break;
        case AHTXX_BUSY_ERROR:
          Serial.println(F("sensor busy, increase polling time"));
          break;
        case AHTXX_ACK_ERROR:
          Serial.println(F("sensor didn't return ACK, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
          break;
        case AHTXX_DATA_ERROR:
          Serial.println(F("received data smaller than expected, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
          break;
        case AHTXX_CRC8_ERROR:
          Serial.println(F("computed CRC8 not match received CRC8, this feature supported only by AHT2x sensors"));
          break;
        default:
          Serial.println(F("unknown status"));
          break;
      }
    }
  public:
    float humidimeter_data[4] = {0};
    void begin() override{
      aht21 = new AHTxx(AHTXX_ADDRESS_X38, AHT2x_SENSOR);
      ENS160 = new DFRobot_ENS160_I2C(&Wire, 0x53);
      while(aht21->begin() != true){
        delay(1000);
        Serial.println(F("Aguardando sensor de umidade."));
      }
      Serial.println(F("AHT20 OK"));
      Serial.println();
      Serial.println(F("DEMO 1: read 12-bytes"));
      ahtTemperature = aht21->readTemperature();
      Serial.print(F("Temperature: "));
      if(ahtTemperature != AHTXX_ERROR){
        Serial.print(ahtTemperature);
        Serial.println(F(" +-0.3C"));
      }
      else{
        printStatus();
        if(aht21->softReset() == true)
          Serial.println(F("reset success"));
        else
          Serial.println(F("reset failed"));
      }
      delay(2000);
      ahtHumidity = aht21->readHumidity();
      Serial.print(F("Humidity: "));
      if (ahtHumidity != AHTXX_ERROR){
        Serial.print(ahtHumidity);
        Serial.println(F(" +-2%"));
      }
      else{
        printStatus();
      }
      delay(200);
      // Serial.println();
      // Serial.println(F("DEMO 2: read 6-byte"));
      // ahtValue = aht21->readTemperature();
      // Serial.print(F("Temperature: "));
      // if(ahtValue != AHTXX_ERROR){
      //   Serial.print(ahtValue);
      //   Serial.println(F(" +-0.3C"));
      // }
      // else{
      //   printStatus();
      // }
      // ahtValue = aht21->readHumidity(AHTXX_USE_READ_DATA);
      // Serial.print(F("Humidity: "));
      // if (ahtValue != AHTXX_ERROR){
      //   Serial.print(ahtValue);
      //   Serial.println(F(" +-2%"));
      // }
      // else{
      //   printStatus();
      // }
      // delay(10000);
      while(NO_ERR != ENS160->begin()){
        delay(1000);
        Serial.println(F("Aguardando o sensor de CO2."));
      }
      Serial.println(F("Begin ok!"));
      ENS160->setPWRMode(ENS160_STANDARD_MODE);
      ENS160->setTempAndHum(ahtTemperature, ahtHumidity);
      uint8_t Status = ENS160->getENS160Status();
      Serial.print(F("Sensor operating status : "));
      Serial.println(Status);
    }
    void gatherData() override{
      uint8_t AQI = ENS160->getAQI();
      Serial.print(F("Air quality index : "));
      Serial.println(AQI);
      uint16_t TVOC = ENS160->getTVOC();
      Serial.print(F("Concentration of total volatile organic compounds : "));
      Serial.print(TVOC);
      Serial.println(F(" ppb"));
      uint16_t ECO2 = ENS160->getECO2();
      Serial.print(F("Carbon dioxide equivalent concentration : "));
      Serial.print(ECO2);
      Serial.println(F(" ppm"));
      Serial.println();
    }
    void printData() override{// Display data for test

    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      for(uint8_t i=0; i<3; i++){
        my_file->print(humidimeter_data[i],4);
        my_file->print(F(","));
      }
    }
};