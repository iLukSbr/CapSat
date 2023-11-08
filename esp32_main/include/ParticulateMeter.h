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

#ifndef PARTICULATE_METER
  #define PARTICULATE_METER 1
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
      HardwareSerial* pmSerial;
    #else// For Arduino
      SoftwareSerial* pmSerial;
    #endif
    Adafruit_PM25AQI* aqi;
    uint16_t particulate_meter_data[PM_SIZE] = {0};
    
  public:
    ParticulateMeter();// Create object
    ~ParticulateMeter();// Release memory
    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    void start() override;
};