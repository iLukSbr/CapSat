#include "pch.h"
#include "UVRadiometer.h"

UVRadiometer::UVRadiometer(){// Create object

}

UVRadiometer::~UVRadiometer(){// Release memory

}

void UVRadiometer::gatherData(){// Get data from component
    double converted_voltage = 1000.f*analogRead(UV_RADIOMETER_PIN)*PIN_MAX_VOLTAGE/PIN_MAX_ANALOG;// Convert analog reading into voltage value
    if(converted_voltage <= 227)// According to voltage (0-1 V)
        uv_radiometer_data = 0;// UV index 0 (low)
    else if(converted_voltage > 227 && converted_voltage <= 318)
        uv_radiometer_data = 1;// UV index 1 (low)
    else if(converted_voltage > 318 && converted_voltage <= 408)
        uv_radiometer_data = 2;// UV index 2 (low)
    else if(converted_voltage > 408 && converted_voltage <= 503)
        uv_radiometer_data = 3;// UV index 3 (medium)
    else if(converted_voltage > 503 && converted_voltage <= 606)
        uv_radiometer_data = 4;// UV index 4 (medium)
    else if(converted_voltage > 606 && converted_voltage <= 696)
        uv_radiometer_data = 5;// UV index 5 (medium)
    else if(converted_voltage > 696 && converted_voltage <= 795)
        uv_radiometer_data = 6;// UV index 6 (high)
    else if(converted_voltage > 795 && converted_voltage <= 881)
        uv_radiometer_data = 7;// UV index 7 (high)
    else if(converted_voltage > 881 && converted_voltage <= 976)
        uv_radiometer_data = 8;// UV index 8 (very high)
    else if(converted_voltage > 976 && converted_voltage <= 1079)
        uv_radiometer_data = 9;// UV index 9 (very hig)
    else if(converted_voltage > 1079 && converted_voltage <= 1170)
        uv_radiometer_data = 10;// UV index 10 (very high)
    else
        uv_radiometer_data = 11;// UV index 11 (extremely high)
}

void UVRadiometer::printData(){// Display data for test
    Serial.print(F("Radiometer: "));
    Serial.println(uv_radiometer_data);
    Serial.println();
}

void UVRadiometer::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(UV_KEY)] = uv_radiometer_data;
}

void UVRadiometer::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(uv_radiometer_data);
    my_file->print(F(","));
}