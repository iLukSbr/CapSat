#include "pch.h"
#include "Humidimeter.h"

Humidimeter::Humidimeter(){// Create object
    aht21 = new AHTxx(AHTXX_ADDRESS_X38, AHT2x_SENSOR);// Instantiate humidity/temperature sensor
    ens160 = new DFRobot_ENS160_I2C(&Wire, 0x53);// Instantiate gas sensor I²C addres 0x53
    while(aht21->begin() != true){// Calibrate AHT21
        delay(CALIBRATION_DELAY);
        Serial.println(F("Waiting for humidity sensor..."));
    }
    float ahtTemperature = aht21->readTemperature();// Get temperature (°C)
    if(ahtTemperature == AHTXX_ERROR)// If temperature error
        ahtTemperature = 0;
    humidimeter_data[0] = aht21->readHumidity();// Get humidity (%)
    if(humidimeter_data[0] == AHTXX_ERROR)// If humidity error
        humidimeter_data[0] = 0;
    while(ens160->begin() != NO_ERR){// Calibrate ENS160
        delay(CALIBRATION_DELAY);
        Serial.println(F("Waiting for CO2 sensor..."));
    }
    ens160->setPWRMode(ENS160_STANDARD_MODE);// Normal operation
    ens160->setTempAndHum(ahtTemperature, humidimeter_data[0]);// Calibrate gas sensor using temperature and humidity
}

Humidimeter::~Humidimeter(){// Release memory
    delete aht21;
    delete ens160;
}

void Humidimeter::gatherData(){// Get data from component
    humidimeter_data[1] = ens160->getAQI();// Air quality index
    humidimeter_data[2] = ens160->getTVOC();// Concentration of total volatile organic compounds (ppb)
    humidimeter_data[3] = ens160->getECO2();// Carbon dioxide equivalent concentration (ppm)
}

void Humidimeter::printData(){// Display data for test
    Serial.print(F("Humidimeter: "));
    for(uint8_t i=0; i<HUMIDIMETER_SIZE; i++){
        Serial.print(humidimeter_data[i]);
        Serial.print(F(" "));
    }
    Serial.println();
}

void Humidimeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(HUMIDITY_KEY)] = humidimeter_data[0];
    payload[F(TVOC_KEY)] = humidimeter_data[1];
    payload[F(CO2_KEY)] = humidimeter_data[2];
    if(!isHTTP)
        payload[F(AQI_KEY)] = humidimeter_data[3];
}

void Humidimeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    for(uint8_t i=0; i<3; i++){
        my_file->print(humidimeter_data[i],4);
        my_file->print(F(","));
    }
}