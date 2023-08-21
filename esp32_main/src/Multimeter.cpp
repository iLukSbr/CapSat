#include "pch.h"
#include "Multimeter.h"

Multimeter::Multimeter(){// Create object
    mult = new INA219_WE(0x40);// I²C address
    while(!mult->init()){// Calibrating
        delay(1000);
        Serial.println(F("Aguardando o voltímetro."));
    }
    mult->setADCMode(SAMPLE_MODE_128);// Mean value of 128 measures
    mult->setMeasureMode(CONTINUOUS);// Continuous data transmission
    mult->setPGain(PG_40);// Max shunt voltage 40 mV
    mult->setShuntSizeInOhms(SHUNT_RESISTANCE);// Shunt resistance
}

Multimeter::~Multimeter(){// Release memory
    delete mult;
}

void Multimeter::gatherData(){// Get data from component
    multimeter_data[0] = (float)map(mult->readRegister(0x02),2,610,234,1253)/100.0;// Voltage (V)
    multimeter_data[1] = constrain((float)map((long)(multimeter_data[0]*100.0), 1000, 1680, 0, 10000)/100.0, 0.0, 100.0);// Battery percentage (%)
    multimeter_data[2] = mult->getCurrent_mA();// Electric current (mA)
    multimeter_data[3] = multimeter_data[2]*multimeter_data[0];// Electric power (mW)
}

void Multimeter::printData(){// Display data for test
    for(uint8_t i=0; i<2; i++){
        Serial.print(multimeter_data[i]);
        Serial.print(F(" "));
    }
    Serial.println();
}

void Multimeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    doc[F(BATTERY_KEY)] = multimeter_data[1];
    if(!isHTTP){
        payload[F(VOLTAGE_KEY)] = multimeter_data[0];
        payload[F(CURRENT_KEY)] = multimeter_data[2];
        payload[F(POWER_KEY)] = multimeter_data[3];
    }
}

void Multimeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<2; i++){
        my_file->print(multimeter_data[i]);
        my_file->print(F(","));
    }
}