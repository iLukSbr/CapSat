#include "pch.h"
#include "Ozonoscope.h"

Ozonoscope::Ozonoscope(){// Create object
    MQ131 = new MQUnifiedsensor(OZONOSCOPE_BOARD, OZONOSCOPE_VOLTAGE_RESOLUTION, OZONOSCOPE_ADC_BIT_RESOLUTION, OZONOSCOPE_PIN, OZONOSCOPE_NAME);
    MQ131->setRegressionMethod(OZONOSCOPE_REGRESSION_METHOD);// Set regression method a*ratio^b
    MQ131->setA(OZONOSCOPE_A_PARAMETER);// Configure the a to to calculate O3 concentration
    MQ131->setB(OZONOSCOPE_B_PARAMETER);// Configure the b to to calculate O3 concentration
    MQ131->init();// Start
    float calcR0 = 0.f;
    for(int i = 1; i<=OZONOSCOPE_CALIBRATION_LOOP; i++){
        MQ131->update();// Update data reading the voltage from the analog pin
        calcR0 += MQ131->calibrate(OZONOSCOPE_CLEAN_AIR_RATIO);// Calibrate
    }
    MQ131->setR0(calcR0/OZONOSCOPE_CALIBRATION_LOOP);// Set R0 and calibrate
    while(isinf(calcR0)){
        delay(CALIBRATION_DELAY);
        Serial.println(F("Waiting for ozonoscope..."));
    }
    while(calcR0 == 0){
        delay(CALIBRATION_DELAY);
        Serial.println(F("Waiting for ozonoscope..."));
    }
    MQ131->serialDebug(true);
    MQ131->setOffset(OZONOSCOPE_OFFSET);
}

Ozonoscope::~Ozonoscope(){// Release memory
    delete MQ131;
}

void Ozonoscope::gatherData(){// Get data from component
    MQ131->update();// Get data
    MQ131->readSensorR0Rs();// Calibrate
    ozonoscope_data = MQ131->getConcentration();// Ozone concentration in air (ppb)
}

void Ozonoscope::printData(){// Display data for test
    Serial.print(F("Ozonoscope: "));
    Serial.print(ozonoscope_data, OZONOSCOPE_DECIMAL_PLACES);
    Serial.println();
}

void Ozonoscope::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(O3_KEY)] = ozonoscope_data;
}

void Ozonoscope::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(ozonoscope_data, OZONOSCOPE_DECIMAL_PLACES);
    my_file->print(F(","));
}