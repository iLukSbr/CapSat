#include "pch.h"
#include "Rainmeter.h"

Rainmeter::Rainmeter(){// Create object
    pinMode(RAINMETER_PIN, INPUT);// Defines RAINMETER_PIN as input
}

Rainmeter::~Rainmeter(){// Release memory
    pinMode(RAINMETER_PIN, OUTPUT);
    digitalWrite(RAINMETER_PIN, LOW);
}

void Rainmeter::gatherData(){// Get data from component
    digitalRead(RAINMETER_PIN) == HIGH ? rainmeter_data=0 : rainmeter_data=1;// HIGH, 0 = dry; LOW, 1 = wet
}

void Rainmeter::printData(){// Display data for test
    Serial.print(F("Sensor de chuva: "));
    Serial.print(rainmeter_data);
    Serial.println();
}

void Rainmeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(RAIN_KEY)] = rainmeter_data;
}

void Rainmeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(rainmeter_data);
    my_file->print(F(","));
}