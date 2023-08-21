#pragma once

#include "Component.h"

// Plantower PMSA003 particulate meter
// https://github.com/adafruit/Adafruit_BusIO
// https://github.com/adafruit/Adafruit_PM25AQI
#include <Adafruit_PM25AQI.h>

#if defined(ESP32) || defined(ESP8266)// For ESP
  // UART hardware interface
  #include <driver/uart.h>
  #include <HardwareSerial.h>
#else// For Arduino
  // UART software interface
  #include <SoftwareSerial.h>
#endif

#define PM_SIZE 12// Sensor data quantity
#define PML1L_KEY "PM1"// JSON PM1 (<1 micrometer diameter) at local pressure key
#define PML2_5L_KEY "PM2.5"// JSON PM2.5 (<2,5 micrometer diameter) at local pressure key
#define PML10L_KEY "PM10"// JSON PM10 (<10 micrometer diameter) at local pressure key
#define PMG0_3_KEY ">0.3"// JSON > 0.3um / 0.1L air key
#define PMG0_5_KEY ">0.5"// JSON > 0.5um / 0.1L air
#define PMG1_KEY ">1"// JSON > 1.0um / 0.1L air
#define PMG2_5_KEY ">2.5"// JSON > 2.5um / 0.1L air
#define PMG5_KEY ">5"// JSON > 5.0um / 0.1L air
#define PMG10_KEY ">10"// JSON > 10 um / 0.1L air
#define PML1S_KEY "PM1"// JSON PM1 (<1 micrometer diameter) at sea level (standard) key
#define PML2_5S_KEY "PM2.5"// JSON JSON PM2.5 (<2,5 micrometer diameter) at sea level (standard) key
#define PML10S_KEY "PM10"// JSON PM10 (<10 micrometer diameter) at sea level (standard) key

#if !defined(ESP32) && !defined(ESP8266)// For Arduino
  #define PM_TX_PIN 2// Particle meter UART RX pin
  #define PM_RX_PIN 3// Particle meter UART TX pin
#endif

class ParticulateMeter : public Component{
  private:
    #if defined(ESP32) || defined(ESP8266)// For ESP
      HardwareSerial* pmSerial = nullptr;
    #else// For Arduino
      SoftwareSerial* pmSerial = nullptr;
    #endif
    Adafruit_PM25AQI* aqi = nullptr;
    uint16_t particulate_meter_data[PM_SIZE] = {0};
    
  public:
    ParticulateMeter();// Create object
    ~ParticulateMeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
};