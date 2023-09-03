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

#pragma once

#include "Component.h"

#if defined(ESP32) || defined(ESP8266)// For ESP
  // UART hardware interface
  #include <driver/uart.h>
  #include <HardwareSerial.h>
#else// For Arduino
  // UART software interface
  #include <SoftwareSerial.h>
#endif

// uBlox GY-GPSV3-NEO-M8N-0-10 GPS
// https://github.com/mikalhart/TinyGPSPlus
#include <TinyGPS++.h>

// Time zone adjustment
// https://github.com/PaulStoffregen/Time
#include <TimeLib.h>

#define ACTUAL_YEAR 2023// Date year validation
#define UTC_OFFSET -3// UTC time zone
#define UTC_GPS_TIME_DRIFT 19// GPS time delayed behind UTC (s)
#define GPS_SIZE 6// Sensor data quantity
#define GPS_DECIMAL_PLACES 6// Decimal places for GPS coordinates
#define GPS_MIN_CHARS_PROCESSED 6// Minimum GPS data received to be considered lost information
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

class Gps : public Component{
  private:
    #if defined(ESP32) || defined(ESP8266)// For ESP
      HardwareSerial* gpsSerial;
    #else// For Arduino
      SoftwareSerial* gpsSerial;
    #endif
    TinyGPSPlus* gps;
    double gps_data[GPS_SIZE] = {0.f};
    
  public:
    Gps();// Create object
    ~Gps();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    const uint16_t getYear() const;// Year
    const uint8_t getMonth() const;// Month
    const uint8_t getDay() const;// Day
    const uint8_t getHour() const;// Hour
    const uint8_t getMinute() const;// Minute
    const uint8_t getSecond() const;// Second
    void gatherDateTime(const bool search);// Get date and time
};