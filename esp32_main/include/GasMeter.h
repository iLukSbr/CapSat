#pragma once

#include "Component.h"

// MiCS-6814 gas meter
// https://github.com/noorkhokhar99/MICS6814
#include <MICS6814.h>

#define GAS_METER_CO_PIN 1// Gas meter analog CO pin
#define GAS_METER_NO2_PIN 2// Gas meter analog NO2 pin
#define GAS_METER_NH3_PIN 3// Gas meter analog NH3 pin
#define GAS_METER_SIZE 8// Sensor data quantity
#define GAS_DECIMAL_PLACES 4// Decimal places for gas concentration

class GasMeter : public Component{
  private:
    MICS6814* gas = nullptr;
  public:
    float gas_meter_data[GAS_METER_SIZE] = {0};
    void begin() override{
      gas = new MICS6814(GAS_METER_CO_PIN, GAS_METER_NO2_PIN, GAS_METER_NH3_PIN);
      gas->calibrate();
    }
    void gatherData() override{
      // Concentrations in ppm
      gas_meter_data[0] = gas->measure(NH3);// Ammonia
      gas_meter_data[1] = gas->measure(CO);// Carbon monoxide
      gas_meter_data[2] = gas->measure(NO2);// Nitrogen dioxide
      gas_meter_data[3] = gas->measure(C3H8);// Propane
      gas_meter_data[4] = gas->measure(C4H10);// Butane
      gas_meter_data[5] = gas->measure(CH4);// Methane
      gas_meter_data[6] = gas->measure(H2);// Hydrogen gas
      gas_meter_data[7] = gas->measure(C2H5OH);// Ethanol
    }
    void printData() override{// Display data for test
      Serial.print(F("Gas meter: "));
      for(uint8_t i=0; i<GAS_METER_SIZE; i++){
        Serial.print(gas_meter_data[i], GAS_DECIMAL_PLACES);
        Serial.print(F(" "));
      }
      Serial.println();
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      for(uint8_t i=0; i<GAS_METER_SIZE; i++){
        my_file->print(gas_meter_data[i], GAS_DECIMAL_PLACES);
        my_file->print(F(","));
      }
    }
};