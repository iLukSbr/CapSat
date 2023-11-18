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

// #define RESET_EEPROM
#define DISABLE_LED

/* Camera Model */
// #define OV5640
// #define OV5640_AF
#define OV2640

// Arduino code
#include <Arduino.h>

// Camera
#include <esp_camera.h>

// Wi-Fi
#include <WiFi.h>

// UART
// #include <driver/uart.h>
// #include <HardwareSerial.h>

// SD Card ESP32
#include <FS.h>
#include <SD_MMC.h>

// Read and write from flash memory
#include <EEPROM.h>

// Disable brownour problems
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
#include <driver/rtc_io.h>

// OV5640 camera 2592x1944 200°
#ifdef OV5640_AF
  #include <ESP32_OV5640_AF.h>
#endif

// Utils
#include <esp_timer.h>
#include <img_converters.h>
#include <StringArray.h>

// Serial web server
// https://github.com/me-no-dev/AsyncTCP
// https://github.com/ayushsharma82/WebSerial
// https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

// Prints
#include "message.h"

#ifdef DISABLE_LED
  #include "esp32-hal-ledc.h"
#endif

/* === Definitions === */
#define SERIAL_BAUD_RATE 115200// Serial baud rate
#define WIFI_SSID "OBSAT_WIFI"// Wi-Fi SSID
#define WIFI_PASSWORD "OBSatZenith1000"// Wi-Fi password
#define CALIBRATION_DELAY 1000// Delay to retry calibration (ms)
#define DEFAULT_PICTURE_DELAY 240000// Delay to take a picture (ms)
#define TRIES_STEPS 30// How many tries

/* === Camera definitions === */
#define HZ_CLK_FREQ 16500000
#define EEPROM_SIZE 255// Define the number of bytes you want to access
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"// Camera models

#ifdef DISABLE_LED
  #define CONFIG_LED_ILLUMINATOR_ENABLED 1// Enable LED FLASH setting

  // LED FLASH setup
  #if CONFIG_LED_ILLUMINATOR_ENABLED
      #define LED_LEDC_CHANNEL 2 //Using different ledc channel/timer than camera
      #define CONFIG_LED_MAX_INTENSITY 0// 0-255
  #endif
#endif


/* === Pointers === */
#ifdef OV5640_AF
  OV5640* ov5640 = nullptr;// Camera
#endif
// HardwareSerial* camSerial = nullptr;// UART for main MCU communication

// Serial web server
AsyncWebServer server(80);

Message msg;

time_t stopwatch = 0;
uint64_t picture_number = 0;
bool new_photo = false;
String path_web = "/1.jpg";// Filename

#ifdef DISABLE_LED
  int led_duty = 1;

  #if CONFIG_LED_ILLUMINATOR_ENABLED
      void enableLed(bool en){// Turn LED On or Off
          int duty = en ? led_duty : 0;
          if (en && (led_duty > CONFIG_LED_MAX_INTENSITY))
              duty = CONFIG_LED_MAX_INTENSITY;
          ledcWrite(LED_LEDC_CHANNEL, duty);
      }
  #endif

  void setupLedFlash(int pin){
      #if CONFIG_LED_ILLUMINATOR_ENABLED
          ledcSetup(LED_LEDC_CHANNEL, 5000, 8);
          ledcAttachPin(pin, LED_LEDC_CHANNEL);
      #endif
  }
#endif

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align:center; }
    .vert { margin-bottom: 10%; }
    .hori{ margin-bottom: 0%; }
    button {
  background-color: #21b555;
  border: none;
  padding: 7px 10px;
  text-align: center;
  font-size: 10px;
  border-radius: 2px;
  width: 50%;
  color: white;
}
  </style>
</head>
<body>
  <div id="container">
    <h2>ESP32-CAM Photo Web Server</h2>
    <p><button onclick="rotatePhoto();">ROTATE PHOTO</button></p>
    <p><button onclick="location.reload();">REFRESH PAGE</button></p>
    <form value="request" onsubmit="requestPicture();">
        <p><input type="number" id="pictureNumber" placeholder="PICTURE NUMBER"></p>
        <p><button type="submit">REQUEST PHOTO</button></p>
    </form>
  </div>
  <div><img src="saved-photo" id="photo" width="100%" height="100%"></div>
</body>
<script>
  var deg = 0;
  function requestPicture(){
    var xhr = new XMLHttpRequest();
    var pictureNumber = document.getElementById("pictureNumber").value;
    var url = "/request?pictureNumber=" + encodeURIComponent(pictureNumber);
    xhr.open('GET', url, true);
    xhr.send();
  }
  function rotatePhoto(){
    var img = document.getElementById("photo");
    deg += 90;
    if(isOdd(deg/90)){document.getElementById("container").className = "vert";}
    else{document.getElementById("container").className = "hori";}
    img.style.transform = "rotate(" + deg + "deg)";
  }
  function isOdd(n){return Math.abs(n % 2) == 1;}
</script>
</html>)rawliteral";

void beginWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(F(WIFI_SSID), F(WIFI_PASSWORD));
  WiFi.setSleep(false);
  while(WiFi.status() != WL_CONNECTED){
    Serial.println(F("Waiting for WiFi connection..."));
    delay(CALIBRATION_DELAY);
  }
  WiFi.setAutoReconnect(true);
  delay(CALIBRATION_DELAY);
  WebSerial.begin(&server);
  delay(CALIBRATION_DELAY);
  server.begin();
  Serial.println(WiFi.localIP());
  Serial.println(F("WiFi OK!"));
  delay(CALIBRATION_DELAY);
}

void setup(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);// Disable brownout detector
  delay(CALIBRATION_DELAY);
  Serial.begin(SERIAL_BAUD_RATE);
  while(!Serial);
  Serial.println(F("ESP32-CAM started!"));
  beginWiFi();
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = HZ_CLK_FREQ;
  config.pixel_format = PIXFORMAT_JPEG;
  delay(5000);
  if(psramFound()){
    msg.multiPrintln(F("PSRAM found!"));
    config.frame_size = FRAMESIZE_UXGA;// Framesizes: QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 1;
  }
  else{
    msg.multiPrintln(F("PSRAM not found!"));
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  while(esp_camera_init(&config) != ESP_OK){
    msg.multiPrint(F("Initializing camera..."));
    delay(CALIBRATION_DELAY);
  }
  #ifdef OV5640_AF
    ov5640 = new OV5640();
    ov5640->start(esp_camera_sensor_get());
    ov5640->focusInit();
    ov5640->autoFocusMode();
  #elif defined(OV5640)
    //
  #elif defined(OV2640)
    //
  #endif
  #ifndef DISABLE_LED
    while(!SD_MMC.begin() || SD_MMC.cardType() == CARD_NONE){
      msg.multiPrintln(F("Initializing MicroSD card..."));
      delay(CALIBRATION_DELAY);
    }
  #endif
  delay(1000);
  #if defined(DISABLE_LED) && defined(LED_GPIO_NUM)
    setupLedFlash(LED_GPIO_NUM);
  #endif
  EEPROM.begin((size_t)EEPROM_SIZE);// initialize EEPROM with predefined size
  #ifdef RESET_EEPROM
    msg.multiPrintln(F("Resetting the EEPROM..."));
    for(byte i=0; i<EEPROM_SIZE; i++){
      msg.multiPrint(F("Resetting EEPROM byte "));
      msg.multiPrintln(i);
      EEPROM.write((int)i, (byte)0);
      EEPROM.commit();
    }
  #endif
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/request", HTTP_GET, [](AsyncWebServerRequest * request){
    if(request->hasParam("pictureNumber")){
      String pictureNumber = request->getParam("pictureNumber")->value();
      path_web = "/" + pictureNumber + ".jpg";
      msg.multiPrint(F("Request received to open picture "));
      msg.multiPrintln(path_web);
    }
    request->send_P(200, "text/html", index_html);
  });
  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request){
    #ifdef DISABLE_LED
      #ifdef LED_GPIO_NUM
        ledcDetachPin(LED_GPIO_NUM);
      #endif
      while(!SD_MMC.begin() || SD_MMC.cardType() == CARD_NONE){
        msg.multiPrintln(F("Initializing MicroSD card..."));
        delay(CALIBRATION_DELAY);
      }
      fs::FS &fs = SD_MMC;
      File photo_file = fs.open(path_web.c_str(), FILE_READ);
      if(photo_file){
          request->send(photo_file, "image/jpeg");
          photo_file.close();
      }
      else
          request->send(404);
      SD_MMC.end();
      // Setup LED FLash if LED pin is defined in camera_pins.h
      #if defined(LED_GPIO_NUM)
        setupLedFlash(LED_GPIO_NUM);
      #endif
    #else
      request->send(SD_MMC, path_web, "image/jpg", false);
    #endif
  });
  server.begin();
  #if defined(DISABLE_LED) && defined(LED_GPIO_NUM)
      setupLedFlash(LED_GPIO_NUM);
  #endif
}

void loop(){
  if(millis() - stopwatch >= DEFAULT_PICTURE_DELAY || !stopwatch){
    byte i = 0;
    unsigned short picture_number = 0;
    msg.multiPrint(F("Web picture path: "));
    msg.multiPrintln(path_web);
    #ifdef OV5640_AF
      while(ov5640->getFWStatus() != FW_STATUS_S_FOCUSED){
          msg.multiPrintln(F("Focusing..."));
          delay(CALIBRATION_DELAY);
          if(++i > TRIES_STEPS){
              msg.multiPrintln(F("Not focused, timeout!"));
              break;
          }
      }
    #endif
    camera_fb_t* fb = esp_camera_fb_get();// Take a picture with camera
    i = 0;
    while(!fb){
      msg.multiPrintln(F("Taking a picture..."));
      delay(CALIBRATION_DELAY);
      if(++i > TRIES_STEPS){
          msg.multiPrintln(F("Picture not taken, timeout!"));
          break;
      }
    }
    if(i <= TRIES_STEPS)
      msg.multiPrintln(F("Picture taken!"));
    stopwatch = millis();
    i = 0;
    msg.multiPrintln(F("Reading EEPROM..."));
    do{
        byte val = EEPROM.read((int)i);
        if(val > 253)
          i++;
        else{
          picture_number = (unsigned short)(val + 1 + 253*i);
          break;
        }
    }while(i < EEPROM_SIZE);
    String path = "/" + String(picture_number) + ".jpg";// Filename
    path_web = path;
    msg.multiPrint(F("EEPROM read! File path: "));
    msg.multiPrintln(path);
    #if defined(DISABLE_LED) && defined(LED_GPIO_NUM)
      ledcDetachPin(LED_GPIO_NUM);
      delay(1000);
      while(!SD_MMC.begin() || SD_MMC.cardType() == CARD_NONE){
        msg.multiPrintln(F("Initializing MicroSD card..."));
        delay(CALIBRATION_DELAY);
      }
    #endif
    fs::FS &fs = SD_MMC;
    File file = fs.open(path.c_str(), FILE_WRITE);
    i = 0;
    msg.multiPrintln(F("Saving photo..."));
    while(!file){
      delay(CALIBRATION_DELAY);
      file = fs.open(path.c_str(), FILE_WRITE);
      if(++i > TRIES_STEPS){
          msg.multiPrintln(F("Photo not saved, timeout!"));
          break;
      }
    }
    delay(CALIBRATION_DELAY);
    file.write(fb->buf, fb->len);// Payload (image), payload length
    delay(10*CALIBRATION_DELAY);
    file.close();
    #ifdef DISABLE_LED
      SD_MMC.end();
      delay(1000);
      #ifdef LED_GPIO_NUM
        setupLedFlash(LED_GPIO_NUM);
      #endif
    #endif
    if(i <= TRIES_STEPS)
      msg.multiPrintln(F("Photo saved!"));
    i = 0;
    while(i<EEPROM_SIZE){
      if(EEPROM.read((int)i) > 253)
        i++;
      else{
        EEPROM.write((int)i, (byte)(picture_number%253));
        EEPROM.commit();
        break;
      }
    }
    esp_camera_fb_return(fb);
    delay(100);
  }
}