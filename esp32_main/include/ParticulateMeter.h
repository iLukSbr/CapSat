#pragma once

#include "Component.h"

// PMSA003 particulate meter
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
#define PM_CALIBRATION_DELAY 1000// Delay between calibration requests (ms)
#define PM_SERIAL_BAUD_RATE 230400// Serial baud rate
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
    ParticulateMeter(){
      #if defined(ESP32) || defined(ESP8266)// For ESP
        pmSerial = new HardwareSerial(UART_NUM_1);
      #else// For Arduino
        pmSerial = new SoftwareSerial(PM_RX_PIN, PM_TX_PIN);
      #endif
      aqi = new Adafruit_PM25AQI();
      pmSerial->begin(PM_SERIAL_BAUD_RATE);
      while(!aqi->begin_UART(pmSerial)){
        delay(PM_CALIBRATION_DELAY);
        Serial.println(F("Waiting for particulate meter..."));
      }
    }
    ~ParticulateMeter(){
      delete pmSerial;
      delete aqi;
    }
    void gatherData() override{
      PM25_AQI_Data data;
      if(aqi->read(&data)){
        // "Less than" particle concentrations at sea level pressure
        particulate_meter_data[0] = data.pm10_standard;// PM1 (<1 micrometer diameter)
        particulate_meter_data[1] = data.pm25_standard;// PM2.5 (<2,5 micrometer diameter)
        particulate_meter_data[2] = data.pm100_standard;// PM10 (<10 micrometer diameter)

        // "Less than" particle concentrations at local pressure
        particulate_meter_data[3] = data.pm10_env;// PM1 (<1 micrometer diameter)
        particulate_meter_data[4] = data.pm25_env;// PM2.5 (<2,5 micrometer diameter)
        particulate_meter_data[5] = data.pm100_env;// PM10 (<10 micrometer diameter)

        // "Greater than" particle concentrations at local pressure
        particulate_meter_data[6] = data.particles_03um;// > 0.3um / 0.1L air
        particulate_meter_data[7] = data.particles_05um;// > 0.5um / 0.1L air
        particulate_meter_data[8] = data.particles_10um;// > 1.0um / 0.1L air
        particulate_meter_data[9] = data.particles_25um;// > 2.5um / 0.1L air
        particulate_meter_data[10] = data.particles_50um;// > 5.0um / 0.1L air
        particulate_meter_data[11] = data.particles_100um;// > 10 um / 0.1L air
      }
    }
    void printData() override{// Display data for test
      Serial.print(F("Particulate meter: "));
      for(uint8_t i=0; i<PM_SIZE; i++){
        Serial.print(particulate_meter_data[i]);
        Serial.print(F(" "));
      }
    }
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override{// Create JSON entries
      payload[F(PML1L_KEY)] = particulate_meter_data[3];// PM1 (<1 micrometer diameter)
      payload[F(PML2_5L_KEY)] = particulate_meter_data[4];// PM2.5 (<2,5 micrometer diameter)
      payload[F(PML10L_KEY)] = particulate_meter_data[5];// PM10 (<10 micrometer diameter)
      payload[F(PMG0_3_KEY)] = particulate_meter_data[6];// > 0.3um / 0.1L air
      payload[F(PMG0_5_KEY)] = particulate_meter_data[7];// > 0.5um / 0.1L air
      payload[F(PMG1_KEY)] = particulate_meter_data[8];// > 1.0um / 0.1L air
      payload[F(PMG2_5_KEY)] = particulate_meter_data[9];// > 2.5um / 0.1L air
      payload[F(PMG5_KEY)] = particulate_meter_data[10];// > 5.0um / 0.1L air
      payload[F(PMG10_KEY)] = particulate_meter_data[11];// > 10 um / 0.1L air
      if(!isHTTP){
        payload[F(PML1S_KEY)] = particulate_meter_data[0] ;// PM1 (<1 micrometer diameter)
        payload[F(PML2_5S_KEY)] = particulate_meter_data[1];// PM2.5 (<2,5 micrometer diameter)
        payload[F(PML10S_KEY)] = particulate_meter_data[2];// PM10 (<10 micrometer diameter)
      }
    }
    void saveCSVToFile(SdFile* my_file) override{// Save data to MicroSD card
      for(uint8_t i=0; i<PM_SIZE; i++){
        my_file->print(particulate_meter_data[i]);
        my_file->print(F(","));
      }
    }
};