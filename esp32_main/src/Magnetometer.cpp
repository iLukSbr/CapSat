#include "pch.h"
#include "Magnetometer.h"

Magnetometer::Magnetometer(){// Create object
    compass = new QMC5883L();// Instantiate compass
    compass->init();// Calibrate moving throug an 8 pattern on a flat surface
    compass->setSamplingRate(50);// Amostras (Hz)
}

Magnetometer::~Magnetometer(){// Release memory
    delete compass;
}

void Magnetometer::gatherData(){// Get data from component
    magnetometer_data = compass->readHeading();// Compass direction (°)
}

void Magnetometer::printData(){// Display data for test
    Serial.print(F("Magnetômetro: "));
    Serial.print(magnetometer_data);
    Serial.println();
}

void Magnetometer::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    if(!isHTTP)
        payload[F(MAGNETOMETER_KEY)] = magnetometer_data;
}

void Magnetometer::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(magnetometer_data);
    my_file->print(F(","));
}