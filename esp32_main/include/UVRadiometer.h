#pragma once

#include "Component.h"

#define UV_RADIOMETER_PIN A4// UV radiometer analog pin

// Taidacent UV radiometer
class UVRadiometer : public Component{
  private:
    uint8_t uv_radiometer_data;
  public:
    void begin() override{}
    void gatherData() override{
      double converted_voltage = 1000.0*analogRead(UV_RADIOMETER_PIN)*5.0/1023.0;
      if(converted_voltage <= 227)
        uv_radiometer_data = 0;
      else if(converted_voltage > 227 && converted_voltage <= 318)
        uv_radiometer_data = 1;
      else if(converted_voltage > 318 && converted_voltage <= 408)
        uv_radiometer_data = 2;
      else if(converted_voltage > 408 && converted_voltage <= 503)
        uv_radiometer_data = 3;
      else if(converted_voltage > 503 && converted_voltage <= 606)
        uv_radiometer_data = 4;
      else if(converted_voltage > 606 && converted_voltage <= 696)
        uv_radiometer_data = 5;
      else if(converted_voltage > 696 && converted_voltage <= 795)
        uv_radiometer_data = 6;
      else if(converted_voltage > 795 && converted_voltage <= 881)
        uv_radiometer_data = 7;
      else if(converted_voltage > 881 && converted_voltage <= 976)
        uv_radiometer_data = 8;
      else if(converted_voltage > 976 && converted_voltage <= 1079)
        uv_radiometer_data = 9;
      else if(converted_voltage > 1079 && converted_voltage <= 1170)
        uv_radiometer_data = 10;
      else
        uv_radiometer_data = 11;
    }
    void printData() override{// Display data for test
      Serial.print(F("Radiometer: "));
      Serial.println(uv_radiometer_data);
      Serial.println();
    }
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
      my_file->print(uv_radiometer_data);
      my_file->print(F(","));
    }
};