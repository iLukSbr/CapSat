#include "pch.h"
#include "Gps.h"

Gps::Gps(){// Create object
    gps = new TinyGPSPlus();
    #if defined(ESP32) || defined(ESP8266)// For ESP
        gpsSerial = new HardwareSerial(UART_NUM_0);
    #else// For Arduino
        gpsSerial = new SoftwareSerial(GPS_RX_PIN, GPS_TX_PIN);
    #endif
    gpsSerial->begin(SERIAL_BAUD_RATE);// Serial baud rate
    do{
        gatherData();
        delay(CALIBRATION_DELAY);
        Serial.println(F("Searching for GPS signal..."));
    }while(!gps->date.isValid() && !gps->time.isValid());// Colecting date and time
}

Gps::~Gps(){// Release memory
    delete gps;
    delete gpsSerial;
}

void Gps::gatherData(){// Get data from component
    uint16_t i = 0;
    while(gpsSerial->available() > 0){
        if(gps->encode(gpsSerial->read())){// Getting data
            if (gps->location.isValid()){
                gps_data[0] = gps->location.lat();// Latitude (°)
                gps_data[1] = gps->location.lng();// Longitude (°)
            }
            if(gps->altitude.isValid())
                gps_data[2] = gps->altitude.meters();// Terrain altitude (m)
            if(gps->speed.isValid())
                gps_data[3] = gps->speed.kmph();// Speed (km/h)
            if(gps->course.isValid())
                gps_data[4] = gps->course.deg();// Course (°)
            if(gps->satellites.isValid())
                gps_data[5] = gps->satellites.value();// Number of GPS satellite signals acquired
        }
        if(i > 5*CALIBRATION_DELAY && gps->charsProcessed() < GPS_MIN_CHARS_PROCESSED){// If data is not valid
            Serial.println(F("GPS signal not detected."));
            break;
        }
        i++;
    }
}

void Gps::printData(){// Display data for test
    Serial.print(F("GPS: "));
    uint8_t i;
    for(i=0; i<2; i++){
        Serial.print(gps_data[i], GPS_DECIMAL_PLACES);
        Serial.print(F(" "));
    }
    for(i=2; i<GPS_SIZE; i++){
        Serial.print(gps_data[i]);
        Serial.print(F(" "));
    }
    Serial.println();
}

void Gps::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    if(!isHTTP){
        payload[F(LATITUDE_KEY)] = gps_data[0];
        payload[F(LONGITUDE_KEY)] = gps_data[1];
        payload[F(SURFACE_ALTITUDE_KEY)] = gps_data[2];
        payload[F(SPEED_KEY)] = gps_data[3];
        payload[F(COURSE_KEY)] = gps_data[4];
        payload[F(SATELLITES_KEY)] = gps_data[5];
    }
}

void Gps::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    uint8_t i;
    for(i=0; i<3; i++){
        my_file->print(gps_data[i], GPS_DECIMAL_PLACES);
        my_file->print(F(","));
    }
    for(i=3; i<GPS_SIZE; i++){
        my_file->print(gps_data[i]);
        my_file->print(F(","));
    }
}

const uint16_t Gps::getYear() const{
    return gps->date.year();
}

const uint8_t Gps::getMonth() const{
    return gps->date.month();
}

const uint8_t Gps::getDay() const{
    return gps->date.day();
}

const uint8_t Gps::getHour() const{
    return gps->time.hour();
}

const uint8_t Gps::getMinute() const{
    return gps->time.minute();
}

const uint8_t Gps::getSecond() const{
    return gps->time.second();
}