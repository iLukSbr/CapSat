/*
MIT License

Copyright (c) 2023 Lucas Yukio Fukuda Matsumoto

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pch.h"
#include "Gps.h"

Gps::Gps():
    signal_status(false),
    gps(new TinyGPSPlus()),
    #if defined(ESP32) || defined(ESP8266)// For ESP
      gpsSerial(new HardwareSerial(UART_NUM_0))
    #else// For Arduino
      gpsSerial(new SoftwareSerial(GPS_RX_PIN, GPS_TX_PIN)) 
    #endif
{// Create object
    gpsSerial->begin(GPS_BAUD_RATE);// Serial baud rate
    start();
}

Gps::~Gps(){// Release memory
    delete gps;
    delete gpsSerial;
}

void Gps::gatherData(){// Get data from component
    multiPrintln(F("Gathering GPS NEO-M8N data..."));
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
            multiPrintln(F("GPS signal not detected."));
            break;
        }
        i++;
    }
}

void Gps::printData(){// Display data for test
    multiPrint(F("GPS NEO-M8N: "));
    uint8_t i;
    for(i=0; i<2; i++){
        multiPrint(gps_data[i], GPS_DECIMAL_PLACES);
        multiPrint(F(" "));
    }
    for(i=2; i<GPS_SIZE; i++){
        multiPrint(gps_data[i]);
        multiPrint(F(" "));
    }
    multiPrintln();
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
    return year();
}

const uint8_t Gps::getMonth() const{
    return month();
}

const uint8_t Gps::getDay() const{
    return day();
}

const uint8_t Gps::getHour() const{
    return hour();
}

const uint8_t Gps::getMinute() const{
    return minute();
}

const uint8_t Gps::getSecond() const{
    return second();
}

void Gps::gatherDateTime(const bool search){// Get date and time, keep searching signal if true
    uint8_t i = 0;
    do{
        gatherData();
        // multiPrintln(F("Searching for GPS signal..."));
        i++;
        if(i>START_TRIES && !search){
            multiPrintln(F("GPS signal not found, timeout!"));
            return;
        }
        delay(CALIBRATION_DELAY);
    }while((!gps->date.isValid() || !gps->time.isValid()) || gps->date.year()>ACTUAL_YEAR);// Colecting date and time
    if(!isStarted())
        started = true;
    if(!signal_status)
        signal_status = true;
    setTime(gps->time.hour(), gps->time.minute(), gps->time.second() + UTC_GPS_TIME_DRIFT, gps->date.day(), gps->date.month(), gps->date.year());
    adjustTime(UTC_OFFSET*SECS_PER_HOUR);
    delay(CALIBRATION_DELAY);
}

void Gps::start(){
    multiPrintln(F("Starting GPS NEO-M8N..."));
    gatherDateTime(false);
    if(isStarted())
        multiPrintln(F("GPS NEO-M8N OK!"));
}

const bool Gps::isSignalAcquired() const{
    return signal_status;
}