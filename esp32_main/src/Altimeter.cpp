#include "pch.h"
#include "Altimeter.h"

Altimeter::Altimeter(){// Create object
    baro = new MS5611();// Instantiate sensor
    baro->begin();// Start sensor
    baro->pressureOffset = MS5611_PRESSURE_OFFSET;// Calibrate according to local air pressure (Pa)
}

Altimeter::~Altimeter(){// Release memory
    delete baro;
}

void Altimeter::gatherData(){// Get data from component
    altimeter_data[0] = baro->getPressure();// Air pressure (Pa)
    altimeter_data[1] = baro->getAltitude();// Altitude (m)
}

void Altimeter::printData(){// Display data for test
    Serial.print(F("Altimeter/barometer: "));
    for(uint8_t i=0; i<ALTIMETER_SIZE; i++){
        Serial.print(altimeter_data[i]);
        Serial.print(F(" "));
    }
    Serial.println();
}

void Altimeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    doc[F(PRESSURE_KEY)] = altimeter_data[0];
    if(!isHTTP)
        payload[F(ALTITUDE_KEY)] = altimeter_data[1];
}

void Altimeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<ALTIMETER_SIZE; i++){
        my_file->print(altimeter_data[i]);
        my_file->print(F(","));
    }
}