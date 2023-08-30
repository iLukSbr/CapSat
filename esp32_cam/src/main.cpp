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
#include <ESP32_OV5640_AF.h>

/* === Definitions === */
#define SERIAL_BAUD_RATE 230400// Serial baud rate
#define WIFI_SSID "OBSAT"// Wi-Fi SSID
#define WIFI_PASSWORD "OBSAT2023"// Wi-Fi password
#define CALIBRATION_DELAY 1000// Delay to retry calibration (ms)
#define DEFAULT_PICTURE_DELAY 60000// Delay to take a picture (ms)
#define PICTURE_SUCCESS 1// Picture taken succesfully code
#define PICTURE_FAILURE 2// Picture failure code
#define DATA_REQUESTED 1// Data and picture request code

/* === Camera definitions === */
#define HZ_CLK_FREQ 6000000
#define EEPROM_SIZE 8// Define the number of bytes you want to access
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"// Camera models

/* === Pointers === */
OV5640* ov5640 = nullptr;// Camera
// HardwareSerial* camSerial = nullptr;// UART for main MCU communication

time_t stopwatch = 0;
uint64_t picture_number = 0;

void beginWiFi(){
  WiFi.begin(F(WIFI_SSID), F(WIFI_PASSWORD));
  WiFi.setSleep(false);
  while(WiFi.status() != WL_CONNECTED){
    delay(CALIBRATION_DELAY);
    Serial.println(F("Waiting for WiFi connection..."));
  }
  WiFi.setAutoReconnect(true);
}

void setup(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);// Disable brownout detector
  Serial.begin(SERIAL_BAUD_RATE);
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
  config.frame_size = FRAMESIZE_VGA;
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;// Framesizes: QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 0;
    config.fb_count = 2;
  }
  else{
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  while(esp_camera_init(&config) != ESP_OK) {
    Serial.print(F("Initializing camera..."));
    delay(CALIBRATION_DELAY);
  }
  ov5640 = new OV5640();
  ov5640->start(esp_camera_sensor_get());
  ov5640->focusInit();
  ov5640->autoFocusMode();
  while(!SD_MMC.begin() || SD_MMC.cardType() == CARD_NONE){
    Serial.println(F("Initializing MicroSD card..."));
    delay(CALIBRATION_DELAY);
  }
  // camSerial = new HardwareSerial(UART_NUM_0);
  // camSerial->begin(SERIAL_BAUD_RATE);
  beginWiFi();
}

void loop(){
  // uint8_t uart_code_received = 0;
  // if(camSerial->available()){// UART communication
  //   while(uart_code_received != 1 && millis() - stopwatch < DEFAULT_PICTURE_DELAY)
  //     uart_code_received = camSerial->read();
  // }
  if(millis() - stopwatch >= DEFAULT_PICTURE_DELAY){
    while(ov5640->getFWStatus() != FW_STATUS_S_FOCUSED)
      Serial.println(F("Focusing..."));
    camera_fb_t* fb = esp_camera_fb_get();// Take a picture with camera
    while(!fb)
      Serial.println(F("Taking a picture..."));
    esp_camera_fb_return(fb);
    EEPROM.begin(EEPROM_SIZE);// initialize EEPROM with predefined size
    picture_number = EEPROM.read(0) + 1;
    String path = "/pictures" + String(picture_number) + ".jpg";// Path where new picture will be saved in SD Card
    fs::FS &fs = SD_MMC;
    File file = fs.open(path.c_str(), FILE_WRITE);
    while(!file){
      Serial.println(F("Saving photo..."));
      delay(CALIBRATION_DELAY);
      file = fs.open(path.c_str(), FILE_WRITE);
    }
    file.write(fb->buf, fb->len);// Payload (image), payload length
    EEPROM.write(0, picture_number);
    EEPROM.commit();
    file.close();
    esp_camera_fb_return(fb);
    // camSerial->println(PICTURE_SUCCESS);
    stopwatch = millis();
  }
  // else
    // camSerial->println(PICTURE_FAILURE);
  // uart_code_received = 0;
}