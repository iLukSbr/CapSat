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

// Multitasking
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include <freertos/semphr.h>
// #include <freertos/queue.h>

// Vector creator
// https://github.com/janelia-arduino/Vector
#include <Vector.h>

#if defined(ESP32) || defined(ESP8266)// For ESP
  // JSON creator
  // https://github.com/bblanchon/ArduinoJson
  #include <ArduinoJson.h>

  // HTTP request
  #include <HTTPClient.h>

  // Serial web server
  // https://github.com/me-no-dev/AsyncTCP
  // https://github.com/ayushsharma82/WebSerial
  // https://github.com/me-no-dev/ESPAsyncWebServer
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <WebSerial.h>
#endif

#include "Message.h"// Messages
#include "componentInclude.h"// Components

/* === Constant values === */
#define TEAM_ID 99// Team ID number
#define SAVE_DELAY 0// Sensors data saving delay (ms)
#define HTTP_SENDING_DELAY 200000// HTTP sending delay (ms)
#ifndef _RELAY
  #define SWITCH_PIN 32// On/off switch pin
#endif
#define I2C_SPEED 9600// Hz

/* === Strings === */
#define WIFI_SSID "obsatserver"// WiFi SSID
#define WIFI_PASSWORD "obsatpass"// WiFi password
#define HTTP_WEBSITE "http://192.168.4.1:80/"// HTTP POST server

// #define WIFI_SSID "termometro"// WiFi SSID
// #define WIFI_PASSWORD "12345678"// WiFi password

// #define WIFI_SSID "OBSAT_WIFI"// WiFi SSID
// #define WIFI_PASSWORD "OBSatZenith1000"// WiFi password
// #define HTTP_WEBSITE "https://obsat.org.br/teste_post/envio.php"// HTTP POST server

#define PAYLOAD_KEY "payload"// JSON payload key
#define TEAM_KEY "equipe"// JSON team key

/* === Components === */
unsigned long stopwatch_sensors = 0;// Stopwatch for timed sensor reading

// Initial configuration
#ifndef _RTCLOCK
  RTClock* ds3231;
#endif
#ifndef _GPS
  Gps* m8n;
#endif
#ifndef _MICROSD_READER_WRITER
  MicroSDReaderWriter* microsd;
#endif

// I²C
#ifndef _ACCELEROMETER
  // Accelerometer* mpu9250;
#endif
#ifndef _ALTIMETER
  // Altimeter* ms5611;
#endif
#ifndef _HUMIDIMETER
  Humidimeter* ens160aht21;
#endif
#ifndef _MAGNETOMETER
  // Magnetometer* qmc5883l;
#endif
#ifndef _MULTIMETER
  // Multimeter* ina219;
#endif

// UART
#ifndef _ESP32_CAMERA
  // ESP32Camera* esp32cam;
#endif
#ifndef _PARTICULATE_METER
  // ParticulateMeter* pmsa003;
#endif

// ADC I²C
#ifndef _GAS_METER
  // GasMeter* mics6814;
#endif
#ifndef _OZONOSCOPE
  Ozonoscope* mq131;
#endif

//Analog
#ifndef _RAINMETER
  // Rainmeter* mhrd;
#endif
#ifndef _THERMOMETER
  Thermometer* ntc;
#endif
#ifndef _UV_RADIOMETER
  // UVRadiometer* taidacent;
#endif

// Serial web server
AsyncWebServer server(80);

// Messages
Message msg;

// TaskHandle_t task_handle = NULL;

/* === Component list === */
Component* storage_array[COMPONENTS_VECTOR_SIZE] = {nullptr};
Vector<Component*> component_list(storage_array);

// TaskHandle_t task_array[COMPONENTS_VECTOR_SIZE] = {nullptr};
// Vector<TaskHandle_t> task_list(task_array);

void pushAll(){
  // Initial configuration
  #ifndef _RTCLOCK
    component_list.push_back(dynamic_cast<Component*>(ds3231 = new RTClock()));
  #endif
  #ifndef _GPS
    component_list.push_back(dynamic_cast<Component*>(m8n = new Gps()));
    if(m8n->isStarted() && m8n->isSignalAcquired())
      ds3231->rtcAdjust(m8n->getYear(), m8n->getMonth(), m8n->getDay(), m8n->getHour(), m8n->getMinute(), m8n->getSecond());
  #endif
  #if !defined(_MICROSD_READER_WRITER) && !defined(_RTCLOCK)
    microsd = new MicroSDReaderWriter(ds3231->getDateTime());// SPI
  #endif

  // UART
  #ifndef _ESP32_CAMERA
    component_list.push_back(dynamic_cast<Component*>(/*esp32cam = */new ESP32Camera()));
  #endif
  #ifndef _PARTICULATE_METER
    component_list.push_back(dynamic_cast<Component*>(/*pmsa003 = */new ParticulateMeter()));
  #endif

  // I²C
  #ifndef _HUMIDIMETER
    component_list.push_back(dynamic_cast<Component*>(ens160aht21 = new Humidimeter()));
  #endif
  #ifndef _MULTIMETER
    component_list.push_back(dynamic_cast<Component*>(/*ina219 = */new Multimeter()));
  #endif
  #ifndef _ACCELEROMETER
    component_list.push_back(dynamic_cast<Component*>(/*mpu9250 = */new Accelerometer()));
  #endif
  #ifndef _ALTIMETER
    component_list.push_back(dynamic_cast<Component*>(/*ms5611 = */new Altimeter()));
  #endif
  #ifndef _MAGNETOMETER
    component_list.push_back(dynamic_cast<Component*>(/*qmc5883l = */new Magnetometer()));
  #endif
  
  // ADC I²C
  #ifndef _GAS_METER
    component_list.push_back(dynamic_cast<Component*>(/*mics6814 = */new GasMeter()));
  #endif
  #ifndef _OZONOSCOPE
    component_list.push_back(dynamic_cast<Component*>(mq131 = new Ozonoscope()));
  #endif

  // Analog
  #ifndef _RAINMETER
    component_list.push_back(dynamic_cast<Component*>(/*mhrd = */new Rainmeter()));
  #endif
  #ifndef _THERMOMETER
    component_list.push_back(dynamic_cast<Component*>(ntc = new Thermometer()));
  #endif
  #ifndef _UV_RADIOMETER
    component_list.push_back(dynamic_cast<Component*>(/*taidacent = */new UVRadiometer()));
  #endif
}

/* === Components start and calibration === */
#if defined(ESP32) || defined(ESP8266)// For ESP
  void beginWiFi(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(F(WIFI_SSID), F(WIFI_PASSWORD));
    WiFi.setSleep(false);
    WiFi.setAutoReconnect(true);
    server.begin();
    WebSerial.begin(&server);
    if(WiFi.status() == WL_CONNECTED)
      msg.multiPrintln(F("WiFi OK!"));
    else
      msg.multiPrintln(F("WiFi connection failed."));
  }
#endif

#ifndef _OZONOSCOPE
  void calibrateMQ131(){
    #ifndef _THERMOMETER
      ntc->gatherData();
      mq131->setClimateParameters(ntc->getTemperature(), ens160aht21->getHumidity());
    #elif !defined(_HUMIDIMETER)
      ens160aht21->gatherData();
      mq131->setClimateParameters(ens160aht21->getTemperature(), ens160aht21->getHumidity());
    #endif
  }
#endif

void beginAll(){
  Wire.begin();
  Wire.setClock(I2C_SPEED);
  #if defined(ESP32) || defined(ESP8266)// For ESP
    beginWiFi();
  #endif
  pushAll();
}

/* === Gather components data === */
void gatherDataAll(){
  #ifndef _OZONOSCOPE
    calibrateMQ131();
  #endif
  if(m8n->isStarted() && m8n->isSignalAcquired())
    ds3231->rtcAdjust(m8n->getYear(), m8n->getMonth(), m8n->getDay(), m8n->getHour(), m8n->getMinute(), m8n->getSecond());
  for(auto element : component_list){
    if(element->isStarted()){
      if(element->isInTime()){
        element->gatherData();
      }
    }
    else{
      element->start();
    }
  }
}

/* === Display gathered data === */
void printAll(){
  for(auto element : component_list){
    if(element->isStarted())
      element->printData();
  }
  msg.multiPrintln();
}

/* === Save gathered data === */
String makeJSONAll(const bool& isHTTP){
  StaticJsonDocument<1000> doc;
  JsonObject payload = doc.createNestedObject(F(PAYLOAD_KEY));
  doc[F(TEAM_KEY)] = TEAM_ID;
  for(auto element : component_list){
    if(element->isStarted())
      element->makeJSON(isHTTP, doc, payload);
  }
  String doc_serialized;
  serializeJson(doc, doc_serialized);
  return doc_serialized;
}

#ifndef _MICROSD_READER_WRITER
  void saveCSVToFileAll(){
    if(microsd->isStarted()){
      SdFile* my_file = microsd->gatherData();// Open file to save into MicroSD card
      if(my_file){
        for(auto element : component_list){
          if(element->isStarted())
            element->saveCSVToFile(my_file);// Save .csv
        }
        my_file->println();// Descend one line
        my_file->close();// Close file
      }
    }
    else
      microsd->start();
  }

  void saveJSONToFileAll(const String& doc_serialized){
    if(microsd->isStarted()){
      SdFile* my_file = microsd->gatherData();// Open file to save into MicroSD card
      if(my_file){
        my_file->println(doc_serialized);// Write JSON
        my_file->println();// Descend one line
        my_file->close();// Close file
      }
      msg.multiPrint(F("JSON saved: "));
      msg.multiPrintln(doc_serialized);
    }
    else
      microsd->start();
  }
#endif

/* === Send JSON data to server by HTTP POST === */
#if defined(ESP32) || defined(ESP8266)// For ESP
  void sendJSONToServerAll(const String& doc_serialized){
    HTTPClient http;
    http.begin(F(HTTP_WEBSITE));
    http.addHeader(F("Content-Type"), F("application/json"));
    uint8_t httpResponseCode = http.POST(doc_serialized);
    if(httpResponseCode > 0){
      msg.multiPrint(F("HTTP POST sending success, code: "));
      msg.multiPrintln(httpResponseCode);
      msg.multiPrint(F("Received: "));
      msg.multiPrintln(http.getString());
    }
    else{
      msg.multiPrint(F("Error occurred while sending HTTP POST: "));
      msg.multiPrintln(http.errorToString(httpResponseCode));
    }
    msg.multiPrint(F("JSON sent: "));
    msg.multiPrintln(doc_serialized);
  }

  unsigned long stopwatch_http = 0;// Stopwatch for timed data sending
#endif

#ifndef _RELAY
  void powerOn3V3(){
    pinMode(SWITCH_PIN, OUTPUT);
    digitalWrite(SWITCH_PIN, HIGH);
    msg.multiPrintln(F("3V3 line ON!"));
  }

  void powerOff3V3(){
    pinMode(SWITCH_PIN, OUTPUT);
    digitalWrite(SWITCH_PIN, LOW);
    msg.multiPrintln(F("3V3 line OFF!"));
  }
#endif

void deleteAll(){
  for(auto element : component_list)
    delete element;
  component_list.clear();
}

// void setReadTasks(){
  // byte i = 0;
  // for(auto element : component_list){
  //   xTaskCreate(
  //     toggleLED,// Function to call
  //     'a' + i,// Task name
  //     1000,// Task size (bytes)
  //     NULL,// Function parameter
  //     1,// Task priority
  //     task_list[i]// Task handle
  //   );
  //   i++;
  // }
// }

/* === Start configuration === */
void setup(){
  Serial.begin(SERIAL_BAUD_RATE);
  while(!Serial);
  #ifndef _RELAY
    powerOn3V3();
  #endif
  Serial.println(F("ESP32 DevKitC started!"));
  beginAll();
  // setReadTasks();
}

/* === Data gathering loop === */
void loop(){
  gatherDataAll();
  if(millis() - stopwatch_sensors > SAVE_DELAY || !stopwatch_sensors || millis()<stopwatch_sensors){
    msg.multiPrint(F("Running time of last data saved: "));
    msg.multiPrintln(stopwatch_sensors);
    stopwatch_sensors = millis();
    #ifndef _MICROSD_READER_WRITER
      #ifdef JSON_FORMAT
        saveJSONToFileAll(makeJSONAll(false));
      #else
        saveCSVToFileAll();
      #endif
    #endif
    printAll();
    #if defined(ESP32) || defined(ESP8266)// For ESP
      if(WiFi.status() == WL_CONNECTED){
        if(millis() - stopwatch_http >= HTTP_SENDING_DELAY || !stopwatch_http){
          msg.multiPrint(F("Running time of last JSON sent: "));
          msg.multiPrintln(stopwatch_http);
          sendJSONToServerAll(makeJSONAll(true));
          stopwatch_http = millis();
        }
      }
    #endif
  }
}