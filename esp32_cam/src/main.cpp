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

// Based on https://github.com/raduprv/esp32-cam_ov2640-timelapse

#include <esp_camera.h>
#include <FS.h>
#include <SD_MMC.h>
#include <driver/rtc_io.h>
#include <EEPROM.h>

// Serial web server
// https://github.com/me-no-dev/AsyncTCP
// https://github.com/ayushsharma82/WebSerial
// https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

/* === Serial and web prints === */
#include "message.h"

// #define RESET_EEPROM

/* === Camera definitions === */
// #define OV2640
#define OV5640_AF
#define CAMERA_MODEL_AI_THINKER
#ifdef OV5640_AF
  #include <ESP32_OV5640_AF.h>
#endif
#define HZ_CLK_FREQ 20000000
#include "camera_pins.h"

/* === ESP32 definitions === */
#define SERIAL_BAUD_RATE 115200// Serial baud rate
#define CPU_FREQUENCY_MHZ 240// CPU frequency in MHz
#define EEPROM_SIZE 512// Define the number of bytes you want to access
#define CALIBRATION_DELAY 500// Delay to retry calibration (ms)
#define MAX_RETRIES_COUNT 10// How many retries
#define uS_TO_S_FACTOR 1000000ULL// Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 300// Time ESP32 will go to sleep (in seconds)
#define TIME_TO_REBOOT 300// Time ESP32 will go to sleep when something goes wrong (in seconds)

/* === WiFi definitions === */
#ifdef WIFI_ENABLED
    #include <WiFi.h>

    // #define WIFI_SSID "OBSAT_WIFI"// Wi-Fi SSID
    // #define WIFI_PASSWORD "OBSatZenith1000"// Wi-Fi password

    #define WIFI_SSID "obsatserver"// WiFi SSID
    #define WIFI_PASSWORD "obsatpass"// WiFi password

    AsyncWebServer server(80);
    String path_web;
#endif

Message msg;

void enableFlash(){
    pinMode(4, INPUT);// GPIO for LED flash
    digitalWrite(4, LOW);
    rtc_gpio_hold_dis(GPIO_NUM_4);// Diable pin hold if it was enabled before sleeping
}

void configureESP(){
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);// Disable brownout detector
    setCpuFrequencyMhz(CPU_FREQUENCY_MHZ);
}

bool configureCamera(){
    camera_config_t config;
    esp_err_t err;
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
    if(psramFound()){// Init with high specs to pre-allocate larger buffers
        msg.multiPrintln("PSRAM found!");
        #ifdef OV5640_AF
            msg.multiPrintln("Defined as OV5640 Auto Focus, using QSXGA.");
            config.frame_size = FRAMESIZE_QSXGA;
        #else
            msg.multiPrintln("Defined as common camera, using UXGA.");
            config.frame_size = FRAMESIZE_UXGA;
        #endif
        config.jpeg_quality = 3;
        config.fb_count = 1;
    }
    else{
        msg.multiPrintln("PSRAM not found!");
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
    #ifdef CAMERA_MODEL_ESP_EYE
        pinMode(13, INPUT_PULLUP);
        pinMode(14, INPUT_PULLUP);
    #endif
    err = esp_camera_init(&config);
    for(byte i=0; err!=ESP_OK && i<MAX_RETRIES_COUNT; i++){
        msg.multiPrintln("Camera init failed!");
        esp_camera_deinit();
        delay(CALIBRATION_DELAY);
        err = esp_camera_init(&config);
    }
    if(err!=ESP_OK){
        return true;
    }
    msg.multiPrintln("Camera initialized");
    return false;
}

void adjustImageParameters(sensor_t **s, camera_fb_t **fb, OV5640 **ov5640){
    int light=0, day_switch_value=140;
    *s = esp_camera_sensor_get();
    (*s)->set_whitebal(*s, 1);// 0 = disable , 1 = enable
    (*s)->set_awb_gain(*s, 1);// 0 = disable , 1 = enable
    (*s)->set_wb_mode(*s, 2);// 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
    (*s)->set_gain_ctrl(*s, 0);// 0 = disable , 1 = enable
    (*s)->set_agc_gain(*s, 0);// 0 to 30
    (*s)->set_gainceiling(*s, (gainceiling_t)6);  // 0 to 6
    (*s)->set_bpc(*s, 1);// 0 = disable , 1 = enable
    (*s)->set_wpc(*s, 1);// 0 = disable , 1 = enable
    (*s)->set_raw_gma(*s, 1);// 0 = disable , 1 = enable
    (*s)->set_lenc(*s, 0);// 0 = disable , 1 = enable
    (*s)->set_hmirror(*s, 0);// 0 = disable , 1 = enable
    (*s)->set_vflip(*s, 0);// 0 = disable , 1 = enable
    (*s)->set_dcw(*s, 0);// 0 = disable , 1 = enable
    (*s)->set_colorbar(*s, 0);// 0 = disable , 1 = enable  
    (*s)->set_reg(*s, 0xff, 0xff, 0x01);// Banksel    
    light = (*s)->get_reg(*s, 0x2f, 0xff);
    if(light < day_switch_value){// Here we are in night mode
        if(light < 45)
            (*s)->set_reg(*s, 0x11, 0xff, 1);// Frame rate (1 means longer exposure)
        (*s)->set_reg(*s, 0x13, 0xff, 0);// Manual everything
        (*s)->set_reg(*s, 0x0c, 0x6, 0x8);// Manual banding
        (*s)->set_reg(*s, 0x45, 0x3f, 0x3f);// Really long exposure
    }
    else{// Here we are in daylight mode
      (*s)->set_reg(*s, 0x2d, 0xff, 0x00);// Extra lines
      (*s)->set_reg(*s, 0x2e, 0xff, 0x00);// Extra lines
      (*s)->set_reg(*s, 0x47, 0xff, 0x00);// Frame Length Adjustment MSBs
    if(light < 150){
      (*s)->set_reg(*s, 0x46, 0xff, 0xd0);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0xff);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0xff);// Exposure
    }
    else if(light < 160){
      (*s)->set_reg(*s, 0x46, 0xff, 0xc0);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0xb0);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    }    
    else if(light < 170){
      (*s)->set_reg(*s, 0x46, 0xff, 0xb0);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x80);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    }    
    else if(light < 180){
      (*s)->set_reg(*s, 0x46, 0xff, 0xa8);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x80);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    } 
    else if(light < 190){ 
      (*s)->set_reg(*s, 0x46, 0xff, 0xa6);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x80);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x90);// Exposure
    } 
    else if(light < 200){
      (*s)->set_reg(*s, 0x46, 0xff, 0xa4);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x80);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    } 
    else if(light < 210){
      (*s)->set_reg(*s, 0x46, 0xff, 0x98);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x60);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    } 
    else if(light < 220){
      (*s)->set_reg(*s, 0x46, 0xff, 0x80);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x20);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    } 
    else if(light < 230){
      (*s)->set_reg(*s, 0x46, 0xff, 0x70);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x20);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    } 
    else if(light < 240){
      (*s)->set_reg(*s, 0x46, 0xff, 0x60);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x20);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0x80);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    } 
    else if(light < 253){
      (*s)->set_reg(*s, 0x46, 0xff, 0x10);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x00);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0x40);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x10);// Exposure
    }
    else{
      (*s)->set_reg(*s, 0x46, 0xff, 0x00);// Frame Length Adjustment LSBs
      (*s)->set_reg(*s, 0x2a, 0xff, 0x00);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0x00);// Line adjust
      (*s)->set_reg(*s, 0x45, 0xff, 0x00);// Exposure
      (*s)->set_reg(*s, 0x10, 0xff, 0x00);// Exposure
    }                          
    (*s)->set_reg(*s, 0x0f, 0xff, 0x4b);
    (*s)->set_reg(*s, 0x03, 0xff, 0xcf);
    (*s)->set_reg(*s, 0x3d, 0xff, 0x34);// Changes the exposure, has to do with frame rate
    (*s)->set_reg(*s, 0x11, 0xff, 0x00);// Frame rate
    (*s)->set_reg(*s, 0x43, 0xff, 0x11);// 11 is the default value     
    }
    #ifdef OV5640_AF
        *ov5640 = new OV5640();
        (*ov5640)->start(*s);
        (*ov5640)->focusInit();
        (*ov5640)->autoFocusMode();
    #endif
    msg.multiPrintln("Getting sensing frame...");
    *fb = esp_camera_fb_get();
    msg.multiPrintln("Got sensing frame!");
    if(light == 0){
      (*s)->set_reg(*s, 0x47, 0xff, 0x40);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0xf0);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
    }
    else if(light == 1){
      (*s)->set_reg(*s, 0x47, 0xff, 0x40);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0xd0);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
    }
    else if(light == 2){
      (*s)->set_reg(*s, 0x47, 0xff, 0x40);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0xb0);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust  
    }
    else if(light == 3){
      (*s)->set_reg(*s, 0x47, 0xff, 0x40);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x70);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust      
    }    
    else if(light == 4){
      (*s)->set_reg(*s, 0x47, 0xff, 0x40);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x40);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust
    }
    else if(light == 5){
      (*s)->set_reg(*s, 0x47, 0xff, 0x20);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x80);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light == 6){
      (*s)->set_reg(*s, 0x47, 0xff, 0x20);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x40);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }         
    else if(light == 7){
      (*s)->set_reg(*s, 0x47, 0xff, 0x20);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x30);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light == 8){
      (*s)->set_reg(*s, 0x47, 0xff, 0x20);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x20);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }     
    else if(light == 9){
      (*s)->set_reg(*s, 0x47, 0xff, 0x20);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x10);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }    
    else if(light == 10){
      (*s)->set_reg(*s, 0x47, 0xff, 0x10);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x70);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light <= 12){
      (*s)->set_reg(*s, 0x47, 0xff, 0x10);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x60);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light <= 14){
      (*s)->set_reg(*s, 0x47, 0xff, 0x10);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x40);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }          
    else if(light <= 18){
      (*s)->set_reg(*s, 0x47, 0xff, 0x08);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0xb0);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light <= 20){
      (*s)->set_reg(*s, 0x47, 0xff, 0x08);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x80);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }      
    else if(light <= 23){
      (*s)->set_reg(*s, 0x47, 0xff, 0x08);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x60);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }           
    else if(light <= 27){
      (*s)->set_reg(*s, 0x47, 0xff, 0x04);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0xd0);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light <= 31){
      (*s)->set_reg(*s, 0x47, 0xff, 0x04);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x80);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }     
    else if(light <= 35){
      (*s)->set_reg(*s, 0x47, 0xff, 0x04);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x60);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }    
    else if(light <= 40){
      (*s)->set_reg(*s, 0x47, 0xff, 0x02);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x70);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light<45){
      (*s)->set_reg(*s, 0x47, 0xff, 0x02);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x40);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light<50){// After this the frame rate is higher, so we need to compensate
      (*s)->set_reg(*s, 0x47, 0xff, 0x04);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0xa0);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }    
    else if(light<55){
      (*s)->set_reg(*s, 0x47, 0xff, 0x04);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x70);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light<65){
      (*s)->set_reg(*s, 0x47, 0xff, 0x04);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x30);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }
    else if(light<75){
      (*s)->set_reg(*s, 0x47, 0xff, 0x02);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x80);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xff);// Line adjust        
    }                 
    else if(light<90){
      (*s)->set_reg(*s, 0x47, 0xff, 0x02);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x50);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0xbf);// Line adjust        
    }
    else if(light<100){
      (*s)->set_reg(*s, 0x47, 0xff, 0x02);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x20);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0x8f);// Line adjust        
    } 
    else if(light<110){
      (*s)->set_reg(*s, 0x47, 0xff, 0x02);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x10);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0x7f);// Line adjust        
    }      
    else if(light<120){
      (*s)->set_reg(*s, 0x47, 0xff, 0x01);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x10);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0x5f);// Line adjust        
    }     
    else if(light<130){
      (*s)->set_reg(*s, 0x47, 0xff, 0x00);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x00);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0x2f);// Line adjust        
    }
    else if(light<140){
      (*s)->set_reg(*s, 0x47, 0xff, 0x00);// Frame Length Adjustment MSBs
      (*s)->set_reg(*s, 0x2a, 0xf0, 0x00);// Line adjust MSB
      (*s)->set_reg(*s, 0x2b, 0xff, 0x00);// Line adjust        
    }
    if(light < day_switch_value)
        (*s)->set_reg(*s, 0x43, 0xff, 0x40);// Magic value to give us the frame faster (bit 6 must be 1)
    (*s)->set_reg(*s, 0xff, 0xff, 0x00);// Banksel 
    (*s)->set_reg(*s, 0xd3, 0xff, 0x8);// Clock
    (*s)->set_reg(*s, 0x42, 0xff, 0x2f);// Image quality (lower is bad)
    (*s)->set_reg(*s, 0x44, 0xff, 3);// Quality
    (*s)->set_reg(*s, 0x92, 0xff, 0x1);// No sharpening
    (*s)->set_reg(*s, 0x93, 0xff, 0x00);// No sharpening
}

bool initSDCard(){//initialize & mount SD card
    byte cardType = CARD_NONE;
    bool sd_status = SD_MMC.begin();
    for(byte i=0; !sd_status && i<MAX_RETRIES_COUNT; i++){
        msg.multiPrintln("Card mount Failed!");
        SD_MMC.end();
        delay(CALIBRATION_DELAY);
        sd_status = SD_MMC.begin();
    }
    if(!sd_status)
        return true;
    cardType = SD_MMC.cardType();
    for(byte i=0; cardType == CARD_NONE && i<MAX_RETRIES_COUNT; i++){
        msg.multiPrintln("No MicroSD card attached!");
        delay(CALIBRATION_DELAY);
        cardType = SD_MMC.cardType();
    }
    if(cardType == CARD_NONE)
        return true;
    return false;
}

bool takePicture(camera_fb_t **fb, OV5640 **ov5640){
    msg.multiPrintln("Taking picture...");
    #ifdef OV5640_AF
        byte focus_status = (*ov5640)->getFWStatus();
        for(byte i=0; i<MAX_RETRIES_COUNT; i++){
            if(focus_status==FW_STATUS_S_FOCUSED){
                msg.multiPrintln("Auto focused!");
                break;
            }
            delay(CALIBRATION_DELAY);
            focus_status = (*ov5640)->getFWStatus();
        }
        if(focus_status!=FW_STATUS_S_FOCUSED){
            msg.multiPrintln("Failed to auto focus!");
        }
    #endif
    if(*fb)
        esp_camera_fb_return(*fb);
    *fb = esp_camera_fb_get();
    for(byte i = 0; !*fb && i<MAX_RETRIES_COUNT; i++){
        msg.multiPrintln("Not having image yet, waiting a bit...");
        *fb = esp_camera_fb_get();
        delay(CALIBRATION_DELAY);
    }
    if(!*fb)
        return true;
    msg.multiPrintln("Got image!");
    return false;
}

void resetSensor(sensor_t **s){// Since we got the frame buffer, we reset the sensor and put it to sleep while saving the file
    (*s)->set_reg(*s, 0xff, 0xff, 0x01);//banksel
    (*s)->set_reg(*s, 0x12, 0xff, 0x80);//reset (we do this to clear the sensor registries, it seems to get more consistent images this way)
    delay(1);
    (*s)->set_reg(*s, 0x09, 0x10, 0x10);//stand by
}

bool savePicture(camera_fb_t **fb){
    int i;
    unsigned short picture_number = 0;
    String path;
    fs::FS &fs = SD_MMC;
    fs::File file;
    for(i=0; i<=EEPROM_SIZE; i++){
        byte val = EEPROM.read(i);
        if((val<=255 && (i+1<EEPROM_SIZE && EEPROM.read(i+1)==0)) || (i==EEPROM_SIZE && val < 255)){
            picture_number = (unsigned short)val + 255*i;
            break;
        }
    }
    if(i > EEPROM_SIZE){
        msg.multiPrintln(F("EEPROM is full!"));
        return true;
    }
    path = "/" + String(picture_number) + ".jpg";// Filename
    msg.multiPrint(F("EEPROM read! File path: "));
    msg.multiPrintln(path);
    file = fs.open(path.c_str(), FILE_WRITE);// Create new file
    for(byte i=0; !file && i<MAX_RETRIES_COUNT; i++){
        msg.multiPrintln("Failed to create file.");
        file.close();
        file = fs.open(path, FILE_WRITE);// Create new file
        delay(CALIBRATION_DELAY);
    }
    if(!file)
        return true;
    if(*fb)
        file.write((*fb)->buf, (*fb)->len);  
    file.close();
    for(i=0; i<=EEPROM_SIZE; i++){
        byte val = EEPROM.read(i);
        if(val < 255){
            EEPROM.write(i, val+1);
            EEPROM.commit();
            break;
        }
    }
    esp_camera_fb_return(*fb);
    msg.multiPrintln("Picture saved!");
    return false;
}

void disableFlash(){
    SD_MMC.end();
    pinMode(4, OUTPUT);              //GPIO for LED flash
    digitalWrite(4, LOW);            //turn OFF flash LED
    rtc_gpio_hold_en(GPIO_NUM_4);    //make sure flash is held LOW in sleep
}

void enterDeepSleepMode(unsigned long long sleep_time_seconds){
  WiFi.disconnect(true, true);
  msg.multiPrint(F("Going to sleep now for "));
  msg.multiPrint(sleep_time_seconds);
  msg.multiPrintln(F(" seconds..."));
  esp_sleep_enable_timer_wakeup(sleep_time_seconds * uS_TO_S_FACTOR);
  Serial.flush();
  esp_deep_sleep_start();
}

void deletePointers(camera_fb_t **fb, sensor_t **s, OV5640 **ov5640){
    delete *fb;
    delete *s;
    delete *ov5640;
}

void failureReboot(camera_fb_t **fb, sensor_t **s, OV5640 **ov5640){
    msg.multiPrintln("Something went wrong, rebooting...");
    deletePointers(&(*fb), &(*s), &(*ov5640));
    disableFlash();
    enterDeepSleepMode(TIME_TO_REBOOT);
}

void configureEEPROM(){
  EEPROM.begin((size_t)EEPROM_SIZE);// Initialize EEPROM with predefined size
  #ifdef RESET_EEPROM
    msg.multiPrintln(F("Resetting the EEPROM..."));
    for(int i=0; i<=EEPROM_SIZE; i++){
      msg.multiPrint(F("Resetting EEPROM byte "));
      msg.multiPrintln(i);
      if(EEPROM.read(i) != 0){
        EEPROM.write(i, (byte)0);
        EEPROM.commit();
      }
    }
  #endif
}

#ifdef WIFI_ENABLED
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
                    <h2>
                        ESP32-CAM Photo Web Server
                    </h2>
                    <p>
                        <button onclick="rotatePhoto();">
                            ROTATE PHOTO
                        </button>
                    </p>
                    <p>
                        <button onclick="location.reload();">
                            REFRESH PAGE
                        </button>
                    </p>
                    <form value="request" onsubmit="requestPicture();">
                        <p>
                            <input type="number" id="picture_number" placeholder="PICTURE NUMBER">
                        </p>
                        <p>
                            <button type="submit">
                                REQUEST PHOTO
                            </button>
                        </p>
                    </form>
                </div>
                <div>
                    <img src="saved-photo" id="photo" width="100%" height="100%">
                </div>
            </body>
            <script>
                var deg = 0;
                function requestPicture(){
                    var xhr = new XMLHttpRequest();
                    var picture_number = document.getElementById("picture_number").value;
                    var url = "/request?picture_number=" + encodeURIComponent(picture_number);
                    xhr.open('GET', url, true);
                    xhr.onload = function(){
                        if (xhr.status === 200){
                            document.getElementById("photo").src = "/saved-photo?" + new Date().getTime();
                        }
                    };
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
        </html>
    )rawliteral";

    void connectToWiFi(){
        Serial.println(F("Waiting for WiFi connection..."));
        WiFi.begin(F(WIFI_SSID), F(WIFI_PASSWORD));
        for(byte i=0; WiFi.status()!=WL_CONNECTED && i<MAX_RETRIES_COUNT; i++)
            delay(CALIBRATION_DELAY);
        if(WiFi.status()!=WL_CONNECTED){
            Serial.println(F("Failed to connect to WiFi!"));
            return;
        }
        Serial.println(F("WiFi connected!"));
    }

    void configureServer(){
        server.begin();
        WebSerial.begin(&server);
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send_P(200, "text/html", index_html);
        });
        server.on("/request", HTTP_GET, [](AsyncWebServerRequest *request){
            if(request->hasParam("picture_number")){
                path_web = "/" + request->getParam("picture_number")->value() + ".jpg";
                msg.multiPrint(F("Request received to open picture "));
                msg.multiPrintln(path_web);
                request->redirect("/saved-photo");
            } else {
                request->send_P(200, "text/html", index_html);
            }
        });
        server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest *request){
            fs::FS &fs = SD_MMC;
            fs::File photo_file = fs.open(path_web.c_str(), FILE_READ);
            if(!photo_file){
                Serial.println("Failed to open file");
                request->send(404);
            }
            else{
                request->send(fs, path_web.c_str(), "image/jpeg");
                photo_file.close();
            }
        });
    }
#endif

void setup(){
    camera_fb_t *fb = nullptr;
    sensor_t *s = nullptr;
    OV5640 *ov5640 = nullptr;
    Serial.begin(SERIAL_BAUD_RATE);
    while(!Serial);
    configureESP();
    enableFlash();
    #ifdef WIFI_ENABLED
        connectToWiFi();
        if(initSDCard())
            failureReboot(&fb, &s, &ov5640);
        if(WiFi.status() == WL_CONNECTED)
            configureServer();
    #endif
    configureEEPROM();
    if(configureCamera())
        failureReboot(&fb, &s, &ov5640);
    adjustImageParameters(&s, &fb, &ov5640);
    if(takePicture(&fb, &ov5640))
        failureReboot(&fb, &s, &ov5640);
    resetSensor(&s);
    #ifndef WIFI_ENABLED
        if(initSDCard())
            failureReboot(&fb, &s, &ov5640);
    #endif
    if(savePicture(&fb))
        failureReboot(&fb, &s, &ov5640);
    disableFlash();
    deletePointers(&fb, &s, &ov5640);
    enterDeepSleepMode(TIME_TO_SLEEP);
}

void loop(){

}