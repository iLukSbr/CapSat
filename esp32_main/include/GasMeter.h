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
#define GAS_METER_KEY "gasometro"// JSON gas meter key
#define NH3_KEY "NH3"// JSON ammonia key
#define CO_KEY "CO"// JSON carbon monoxide key
#define NO2_KEY "NO2"// JSON nitrogen dioxide key
#define C3H8_KEY "C3H8"// JSON propane concentration key key
#define C4H10_KEY "C4H10"// JSON butane concentration key
#define CH4_KEY "CH4"// JSON methane concentration key
#define H2_KEY "H2"// JSON hydrogen gas concentration key
#define C2H5OH_KEY "C2H5OH"// JSON ethanol concentration key

class GasMeter : public Component{
  private:
    MICS6814* gas = nullptr;
    float gas_meter_data[GAS_METER_SIZE] = {0.f};
  public:
    GasMeter(){
      gas = new MICS6814(GAS_METER_CO_PIN, GAS_METER_NO2_PIN, GAS_METER_NH3_PIN);
      gas->calibrate();
    }
    ~GasMeter(){
      delete gas;
    }
    void gatherData() override{
      // Concentrations in ppm
      gas_meter_data[0] = gas->measure(NH3);// Ammonia concentration in air (ppm)
      gas_meter_data[1] = gas->measure(CO);// Carbon monoxide concentration in air (ppm)
      gas_meter_data[2] = gas->measure(NO2);// Nitrogen dioxide concentration in air (ppm)
      gas_meter_data[3] = gas->measure(C3H8);// Propane concentration in air (ppm)
      gas_meter_data[4] = gas->measure(C4H10);// Butane concentration in air (ppm)
      gas_meter_data[5] = gas->measure(CH4);// Methane concentration in air (ppm)
      gas_meter_data[6] = gas->measure(H2);// Hydrogen gas concentration in air (ppm)
      gas_meter_data[7] = gas->measure(C2H5OH);// Ethanol concentration in air (ppm)
    }
    void printData() override{// Display data for test
      Serial.print(F("Gas meter: "));
      for(uint8_t i=0; i<GAS_METER_SIZE; i++){
        Serial.print(gas_meter_data[i], GAS_DECIMAL_PLACES);
        Serial.print(F(" "));
      }
      Serial.println();
    }
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
      payload[F(NH3_KEY)] = gas_meter_data[0];// Ammonia
      payload[F(CO_KEY)] = gas_meter_data[1];// Carbon monoxide
      payload[F(NO2_KEY)] = gas_meter_data[2];// Nitrogen dioxide
      payload[F(C3H8_KEY)] = gas_meter_data[3];// Propane
      payload[F(C4H10_KEY)] = gas_meter_data[4];// Butane
      payload[F(CH4_KEY)] = gas_meter_data[5];// Methane
      payload[F(H2_KEY)] = gas_meter_data[6];// Hydrogen gas
      payload[F(C2H5OH_KEY)] = gas_meter_data[7];// Ethanol
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
      for(uint8_t i=0; i<GAS_METER_SIZE; i++){
        my_file->print(gas_meter_data[i], GAS_DECIMAL_PLACES);
        my_file->print(F(","));
      }
    }
};