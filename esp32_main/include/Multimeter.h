#pragma once

#include "Component.h"

// I²C interface
#include <Wire.h>

// INA219 multimeter
// https://github.com/wollewald/INA219_WE
#include <INA219_WE.h>// Address 0x40

class Multimeter : public Component{
  private:
    INA219_WE tensao = INA219_WE(0x40);// Address I²C
  public:
    float multimeter_data[2] = {0};
    void begin() override{
      while(!tensao.init()){// Aguardando calibração
        delay(1000);
        Serial.println(F("Aguardando o voltímetro."));
      }
      tensao.setADCMode(SAMPLE_MODE_128);// Leitura da média de 128 medidas
      tensao.setMeasureMode(CONTINUOUS);// Transmissão contínua de dados
      tensao.setPGain(PG_40);// Tensão do shunt 40 mV
      tensao.setShuntSizeInOhms(0.01);// Resistência do shunt 10R
    }
    void gatherData() override{
      multimeter_data[0] = (float)map(tensao.readRegister(0x02),2,610,234,1253)/100.0;// Tensão elétrica em volts calibrada
      multimeter_data[1] = constrain((float)map((long)(multimeter_data[0]*100.0), 1000, 1680, 0, 10000)/100.0, 0.0, 100.0);// Porcentagem da bateria calibrada
    }
    void printData() override{// Display data for test
      for(uint8_t i=0; i<2; i++){
        Serial.print(multimeter_data[i]);
        Serial.print(F(" "));
      }
      Serial.println();
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      for(uint8_t i=0; i<2; i++){
        my_file->print(multimeter_data[i]);
        my_file->print(F(","));
      }
    }
};