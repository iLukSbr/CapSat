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

#include "pch.h"

#include <Arduino.h>// Arduino compatibility

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
// ESP32-CAM
#include "ESP32Camera.h"

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
#define COMPONENTS_VECTOR_SIZE 14// Components quantity inside vector
#define SERIAL_BAUD_RATE 230400// Serial baud rate
#define RELAY_PIN 10// Relay pin

/* === Strings === */
#define WIFI_SSID "OBSAT"// WiFi SSID
#define WIFI_PASSWORD "OBSAT2023"// WiFi password
#define HTTP_WEBSITE "https://obsat.org.br/teste_post/envio.php"// HTTP POST website
#define PAYLOAD_KEY "payload"// JSON payloade key
#define TEAM_KEY "equipe"// JSON team key

/* === Components instantiation === */
Accelerometer* mpu9250;
Altimeter* ms5611;
ESP32Camera* esp32cam;
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
  m8n = new Gps();// UART
  mhrtc2 = new RTClock(m8n->getYear(), m8n->getMonth(), m8n->getDay(), m8n->getHour(), m8n->getMinute(), m8n->getSecond());// ThreeWire
  microsd = new MicroSDReaderWriter(mhrtc2->getDateTime());// SPI

  // UART
  pmsa003 = new ParticulateMeter();
  esp32cam = new ESP32Camera();

  // I²C
  mpu9250 = new Accelerometer();
  ms5611 = new Altimeter();
  qmc5883l = new Magnetometer();
  ens160aht21 = new Humidimeter();
  ina219 = new Multimeter();

  // Analog
  mhrd = new Rainmeter();
  mics6814 = new GasMeter();
  mq131 = new Ozonoscope();
  taidacent = new UVRadiometer();
  ntc = new Thermometer();
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
  component_list.push_back(dynamic_cast<Component*>(ens160aht21));
  component_list.push_back(dynamic_cast<Component*>(ntc));
  component_list.push_back(dynamic_cast<Component*>(mq131));
  component_list.push_back(dynamic_cast<Component*>(mhrd));
  component_list.push_back(dynamic_cast<Component*>(mics6814));
  component_list.push_back(dynamic_cast<Component*>(taidacent));
  component_list.push_back(dynamic_cast<Component*>(mhrd));
}

/* === Components start and calibration === */
void beginI2C(){
  Wire.begin();// I²C start
  Wire.setClock(400000);// I²C communication frequency 400 kHz
}

#if defined(ESP32) || defined(ESP8266)// For ESP
  void beginWiFi(){
    WiFi.begin(F(WIFI_SSID), F(WIFI_PASSWORD));
    WiFi.setSleep(false);
    while (WiFi.status() != WL_CONNECTED){
      delay(1000);
      Serial.println(F("Waiting for WiFi connection..."));
    }
  }
#endif

void beginAll(){
  beginI2C();
  newAll();
  ens160aht21->gatherData();
  ntc->gatherData();
  mq131->setClimateParameters(ntc->getTemperature(), ens160aht21->getHumidity());
  #if defined(ESP32) || defined(ESP8266)// For ESP
    beginWiFi();
  #endif
}

/* === Gather components data === */
void gatherDataAll(){
  for(auto element : component_list)
    element->gatherData();
  mq131->setClimateParameters(ntc->getTemperature(), ens160aht21->getHumidity());
}

/* === Display gathered data === */
void printAll(){
  for(auto element : component_list)
    element->printData();
  Serial.println();
}

/* === Save gathered data === */
String makeJSONAll(const bool& isHTTP){
  StaticJsonDocument<1000> doc;
  JsonObject payload = doc.createNestedObject(F(PAYLOAD_KEY));
  doc[F(TEAM_KEY)] = TEAM_ID;
  for(auto element : component_list)
    element->makeJSON(isHTTP, doc, payload);
  String doc_serialized;
  serializeJson(doc, doc_serialized);
  return doc_serialized;
}

void saveCSVToFileAll(){
  SdFile* my_file = microsd->gatherData();// Open file to save into MicroSD card
  if(my_file){
    for(auto element : component_list)
      element->saveCSVToFile(my_file);// Save .csv
    my_file->println();// Descend one line
    my_file->close();// Close file
  }
}

void saveJSONToFileAll(const String& doc_serialized){
  SdFile* my_file = microsd->gatherData();// Open file to save into MicroSD card
  if(my_file){
    my_file->println(doc_serialized);// Write JSON
    my_file->println();// Descend one line
    my_file->close();// Close file
  }
}

/* === Send JSON data to server by HTTP POST === */
#if defined(ESP32) || defined(ESP8266)// For ESP
  void sendJSONToServerAll(String doc_serialized){
    HTTPClient http;
    http.begin(F(HTTP_WEBSITE));
    http.addHeader(F("Content-Type"), F("application/json"));
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

void powerOnComponents(){
  digitalWrite(RELAY_PIN, HIGH);
}

void powerOffComponents(){
  digitalWrite(RELAY_PIN, LOW);
}

void deleteAll(){
  for(auto element : component_list)
    delete element;
  component_list.clear();
}

/* === Start configuration === */
void setup(){
  pinMode(RELAY_PIN, OUTPUT);
  powerOnComponents();
  Serial.begin(SERIAL_BAUD_RATE);
  delay(CALIBRATION_DELAY);
  beginAll();
  delay(CALIBRATION_DELAY);
}

/* === Data gathering loop === */
void loop(){
  gatherDataAll();
  #ifdef JSON_FORMAT
    saveJSONToFileAll(makeJSONAll(false));
  #else
    saveCSVToFileAll();
  #endif
  printAll();
  #if defined(ESP32) || defined(ESP8266)// For ESP
    unsigned long elapsed_time = millis();
    if(elapsed_time - stopwatch >= HTTP_SENDING_DELAY || stopwatch <= HTTP_SENDING_DELAY){
      sendJSONToServerAll(makeJSONAll(true));
      stopwatch = millis();
    }
  #endif
  delay(CALIBRATION_DELAY);
}