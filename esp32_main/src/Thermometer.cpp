#include "pch.h"
#include "Thermometer.h"

Thermometer::Thermometer(){// Create object
    originThermistor = new NTC_Thermistor(NTC_SENSOR_PIN, NTC_REFERENCE_RESISTANCE, NTC_NOMINAL_RESISTANCE, NTC_NOMINAL_TEMPERATURE, NTC_B_VALUE);
    thermistor = new SmoothThermistor(originThermistor, NTC_SMOOTHING_FACTOR);
}

Thermometer::~Thermometer(){// Release memory
    delete originThermistor;
    delete thermistor;
}

void Thermometer::gatherData(){// Get data from component
    thermometer_data = thermistor->readCelsius();// Analog voltage reading and conversion to temperature in Celsius degree (°)
}

void Thermometer::printData(){// Display data for test
    Serial.print(F("Termômetro: "));
    Serial.print(thermometer_data);
    Serial.println();
}

void Thermometer::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    doc[F(TEMPERATURE_KEY)] = thermometer_data;
}

void Thermometer::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(thermometer_data);
    my_file->print(F(","));
}