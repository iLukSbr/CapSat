/* === CapSat CubeSat code === */
/* === 2ª OBSAT MCTI - 2023 === */
/* === 2nd Brazilian Satellite Olympiad === */
/* === Author: Lucas Yukio Fukuda Matsumoto === */

/* === Team === */
/*
  Tutor: Prof. Dr. Angela Cristina Cararo (PUCPR)
  Students:
  Eduardo Ferronato Pelle Guin (PUCPR)
  Lucas Yukio Fukuda Matsumoto (UTFPR)
  Thiago Augusto Pino Gomes (PUCPR)

  PUCPR = Pontifícia Universidade Católica do Paraná
  UTFPR = Universidade Tecnológica Federal do Paraná
*/

/*
    Copyright (C) <2023>  <Lucas Yukio Fukuda Matsumoto>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "pch.h"

// Vector creator
// https://github.com/janelia-arduino/Vector
#include <Vector.h>

#if defined(ESP32) || defined(ESP8266)// For ESP
  // JSON creator
  // https://github.com/bblanchon/ArduinoJson
  #include <ArduinoJson.h>

  // HTTP request
  #include <HTTPClient.h>

  // WiFi connection
  #include <WiFi.h>
#endif

/* === UART Interface === */
// PMSA003 particulate meter
#include "ParticulateMeter.h"

// NEO-M8N GPS
#include "Gps.h"

/* === ThreeWire Interface === */
// MH-RTC 2 clock
#include "RTClock.h"

/* === I²C Interface === */
// MPU-9250 accelerometer/gyroscope
#include "Accelerometer.h"

// MS5611 altimeter/barometer
#include "Altimeter.h"

// QMC5883L magnetometer
#include "Magnetometer.h"

// ENS160+AHT21 carbodioximeter, TVOC meter and humidimeter
#include "Humidimeter.h"

// INA219 multimeter
#include "Multimeter.h"

/* === Analog Interface === */
// MiCS-6814 gas meter
#include "GasMeter.h"

// MQ-131 ozonoscope
#include "Ozonoscope.h"

// NTC thermometer
#include "Thermometer.h"

// Rain sensor
#include "Rainmeter.h"

// Taidacent UV level sensor
#include "UVRadiometer.h"

/* === Constant values === */
#define TEAM_ID 99// Team ID number
#define HTTP_SENDING_DELAY 4000// HTTP sending delay (ms)
#define COMPONENTS_VECTOR_SIZE 13// Components quantity inside vector
#define SERIAL_BAUD_RATE 230400// Serial baud rate

/* === Strings === */
#define WIFI_SSID "OBSAT"// WiFi SSID
#define WIFI_PASSWORD "OBSAT2023"// WiFi password
#define HTTP_WEBSITE "https://obsat.org.br/teste_post/envio.php"// HTTP POST website

/* === Components instantiation === */
Accelerometer* mpu9250;
Altimeter* ms5611;
Rainmeter* mhrd;
GasMeter* mics6814;
Gps* m8n;
MicroSDReaderWriter* microsd;
Magnetometer* qmc5883l;
Ozonoscope* mq131;
ParticulateMeter* pmsa003;
UVRadiometer* taidacent;
RTClock* mhrtc2;
Thermometer* ntc;
Humidimeter* ens160aht21;
Multimeter* ina219;

void newAll(){
  mpu9250 = new Accelerometer();
  ms5611 = new Altimeter();
  mhrd = new Rainmeter();
  mics6814 = new GasMeter();
  m8n = new Gps();
  microsd = new MicroSDReaderWriter();
  qmc5883l = new Magnetometer();
  mq131 = new Ozonoscope();
  pmsa003 = new ParticulateMeter();
  taidacent = new UVRadiometer();
  mhrtc2 = new RTClock();
  ntc = new Thermometer();
  ens160aht21 = new Humidimeter();
  ina219 = new Multimeter();
}

/* === Component list === */
Component* storage_array[COMPONENTS_VECTOR_SIZE] = {nullptr};
Vector<Component*> component_list(storage_array);

void pushAll(){
  component_list.push_back(dynamic_cast<Component*>(m8n));
  component_list.push_back(dynamic_cast<Component*>(mhrtc2));
  component_list.push_back(dynamic_cast<Component*>(pmsa003));
  component_list.push_back(dynamic_cast<Component*>(mpu9250));
  component_list.push_back(dynamic_cast<Component*>(ms5611));
  component_list.push_back(dynamic_cast<Component*>(ina219));
  component_list.push_back(dynamic_cast<Component*>(qmc5883l));
  component_list.push_back(dynamic_cast<Component*>(mhrd));
  component_list.push_back(dynamic_cast<Component*>(mics6814));
  component_list.push_back(dynamic_cast<Component*>(taidacent));
  component_list.push_back(dynamic_cast<Component*>(mq131));
  component_list.push_back(dynamic_cast<Component*>(ntc));
  component_list.push_back(dynamic_cast<Component*>(mhrd));
}

/* === Components start and calibration === */
void beginI2C(){
  Wire.begin();// I²C start
  Wire.setClock(400000);// I²C communication frequency 400 kHz
}

#if defined(ESP32) || defined(ESP8266)// For ESP
  void beginWiFi(){
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.setSleep(false);
    while (WiFi.status() != WL_CONNECTED){
      delay(1000);
      Serial.println("Waiting for WiFi connection...");
    }
  }
#endif

void beginAll(){
  beginI2C();
  newAll();
  pushAll();
  for(auto element : component_list)
    element->begin();
  microsd->begin(mhrtc2->clock_data);

  #if defined(ESP32) || defined(ESP8266)// For ESP
    beginWiFi();
  #endif
}

/* === Gather components data === */
void gatherDataAll(){
  for(auto element : component_list)
    element->gatherData();
}

/* === Save gathered data === */
void saveAll(){
  SdFile* my_file = microsd->gatherData();// Abre o my_file para salvar no cartão MicroSD
  if(my_file)
    for(auto element : component_list)
      element->saveData(my_file);
  my_file->println();
  my_file->close();
}

/* === Display gathered data === */
void printAll(){
  for(auto element : component_list)
    element->printData();
  Serial.println();
}

/* === Format data as JSON === */
#if defined(ESP32) || defined(ESP8266)// For ESP
  String makeJSON(){
    StaticJsonDocument<1000> doc;
    doc["equipe"] = TEAM_ID;
    doc["bateria"] = 98.5;
    doc["temperatura"] = 15.4;
    doc["pressao"] = 101300;
    doc["giroscopio"][0] = 42;
    doc["giroscopio"][1] = 90;
    doc["giroscopio"][2] = 30;
    doc["acelerometro"][0] = 10;
    doc["acelerometro"][1] = 3;
    doc["acelerometro"][2] = 4;
    JsonObject payload = doc.createNestedObject("payload");
    payload["gas"] = 3.1415;
    payload["mag"][0] = 12;
    payload["mag"][1] = 123;
    payload["mag"][2] = 543;
    payload["chuva"] = 0;
    String doc_serialized;
    serializeJson(doc, doc_serialized);
    return doc_serialized;
  }

/* === Send JSON data to server by HTTP POST === */
  void sendDataToServer(String doc_serialized){
    HTTPClient http;
    http.begin(HTTP_WEBSITE);
    http.addHeader("Content-Type", "application/json");
    uint8_t httpResponseCode = http.POST(doc_serialized);
    if(httpResponseCode > 0){
      Serial.print(F("HTTP POST sending success: "));
      Serial.println(httpResponseCode);
      Serial.println(http.getString());
    }
    else{
      Serial.print(F("Error occurred while sending HTTP POST: "));
      Serial.println(http.errorToString(httpResponseCode));
    }
  }

  unsigned long stopwatch = 0;// Stopwatch for timed data sending
#endif

/* === Start configuration === */
void setup(){
  Serial.begin(SERIAL_BAUD_RATE);
  beginAll();
  delay(1000);
}

/* === Data gathering loop === */
void loop(){
  gatherDataAll();
  saveAll();
  printAll();
  delay(1000);

  #if defined(ESP32) || defined(ESP8266)// For ESP
    unsigned long elapsed_time = millis();
    if(elapsed_time - stopwatch >= HTTP_SENDING_DELAY || stopwatch <= HTTP_SENDING_DELAY){
      sendDataToServer(makeJSON());
      stopwatch = millis();
    }
  #endif
}