#pragma once

#include "Component.h"

// NEO-M8N GPS
// https://github.com/mikalhart/TinyGPSPlus
#include <TinyGPS++.h>

#define GPS_SIZE 6// Sensor data quantity
#define GPS_SERIAL_BAUD_RATE 230400// Serial baud rate
#define GPS_DECIMAL_PLACES 6// Decimal places for GPS coordinates
#define GPS_MIN_CHARS_PROCESSED 6// Minimum GPS data received to be considered lost information
#define GPS_CALIBRATION_DELAY 1000// Delay between calibration requests (ms)
#define LATITUDE_KEY "latitude"// JSON latitude key
#define LONGITUDE_KEY "longitude"// JSON longitude key
#define SURFACE_ALTITUDE_KEY "altitude do terreno"// JSON terrain altitude key
#define SPEED_KEY "velocidade"// JSON speed key
#define COURSE_KEY "curso"// JSON course key
#define SATELLITES_KEY "satelites"// JSON number of GPS satellites key

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
    double gps_data[GPS_SIZE] = {0.f};
    void getInfo(){// Read values
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
  public:
    Gps(){
      gps = new TinyGPSPlus();
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
    ~Gps(){
      delete gps;
      delete gpsSerial;
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
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
      if(!isHTTP){
        payload[F(LATITUDE_KEY)] = gps_data[0];
        payload[F(LONGITUDE_KEY)] = gps_data[1];
        payload[F(SURFACE_ALTITUDE_KEY)] = gps_data[2];
        payload[F(SPEED_KEY)] = gps_data[3];
        payload[F(COURSE_KEY)] = gps_data[4];
        payload[F(SATELLITES_KEY)] = gps_data[5];
      }
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
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
    const uint16_t getYear(){
      return gps->date.year();
    }
    const uint8_t getMonth(){
      return gps->date.month();
    }
    const uint8_t getDay(){
      return gps->date.day();
    }
    const uint8_t getHour(){
      return gps->time.hour();
    }
    const uint8_t getMinute(){
      return gps->time.minute();
    }
    const uint8_t getSecond(){
      return gps->time.second();
    }
};