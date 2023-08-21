#pragma once

#include "Component.h"

// NEO-M8N GPS
// https://github.com/mikalhart/TinyGPSPlus
#include <TinyGPS++.h>

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
    
  public:
    Gps();// Create object
    ~Gps();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    uint16_t getYear() const;// Year
    uint8_t getMonth() const;// Month
    uint8_t getDay() const;// Day
    uint8_t getHour() const;// Hour
    uint8_t getMinute() const;// Minute
    uint8_t getSecond() const;// Second
};