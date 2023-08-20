#pragma once

#include "Component.h"

// NEO-M8N GPS
// https://github.com/mikalhart/TinyGPSPlus
#include <TinyGPS++.h>

#define GPS_SIZE 5// Sensor data quantity
#define GPS_SERIAL_BAUD_RATE 230400// Serial baud rate
#define GPS_DECIMAL_PLACES 6// Decimal places for GPS coordinates
#define GPS_MIN_CHARS_PROCESSED 6// Minimum GPS data received to be considered lost information
#define GPS_CALIBRATION_DELAY 1000// Delay between calibration requests (ms)

#if !defined(ESP32) && !defined(ESP8266)// For Arduino
  #define GPS_TX_PIN 8// GPS UART RX pin
  #define GPS_RX_PIN 9// GPS UART TX pin
#endif

#if defined(ESP32) || defined(ESP8266)// For ESP
  // UART hardware interface
  #include <driver/uart.h>
  #include <HardwareSerial.h>
#else// For Arduino
  // UART software interface
  #include <SoftwareSerial.h>
#endif

class Gps : public Component{
  private:
    #if defined(ESP32) || defined(ESP8266)// For ESP
      HardwareSerial* gpsSerial = nullptr;
    #else// For Arduino
      SoftwareSerial* gpsSerial = nullptr;
    #endif
    TinyGPSPlus* gps = nullptr;
    void getInfo(){
      if (gps->location.isValid()){
        gps_data[0] = gps->location.lat();// Latitude (°)
        gps_data[1] = gps->location.lng();// Longitude (°)
      }
      if(gps->altitude.isValid())
        gps_data[2] = gps->altitude.meters();// Surface altitude (m)
      if(gps->speed.isValid())
        gps_data[3] = gps->speed.kmph();// Speed (km/h)
      if(gps->course.isValid())
        gps_data[4] = gps->course.deg();// Direction (°)
    }
  public:
    double gps_data[GPS_SIZE] = {0};
    void begin() override{
      #if defined(ESP32) || defined(ESP8266)// For ESP
        gpsSerial = new HardwareSerial(UART_NUM_0);
      #else// For Arduino
        gpsSerial = new SoftwareSerial(GPS_RX_PIN, GPS_TX_PIN);
      #endif
      gpsSerial->begin(GPS_SERIAL_BAUD_RATE);// Serial baud rate
      do{
        gatherData();
        delay(GPS_CALIBRATION_DELAY);
        Serial.println(F("Searching for GPS signal..."));
      }while(!gps->date.isValid() && !gps->time.isValid());// Colecting date and time
    }
    void gatherData() override{
      uint16_t i = 0;
      while(gpsSerial->available() > 0){
        if(gps->encode(gpsSerial->read()))// Getting data
          getInfo();
        if(i > 5*GPS_CALIBRATION_DELAY && gps->charsProcessed() < GPS_MIN_CHARS_PROCESSED){// If data is not valid
          Serial.println(F("GPS signal not detected."));
          break;
        }
        i++;
      }
    }
    void printData() override{// Display data for test
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
    void saveData(SdFile* my_file) override{// Save data to MicroSD card
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
};