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

#include "Component.h"

#ifndef ESP32_CAMERA
  #define ESP32_CAMERA 1
#endif

#if defined(ESP32) || defined(ESP8266)// For ESP
  // UART hardware interface
  #include <driver/uart.h>
  #include <HardwareSerial.h>
#else// For Arduino
  // UART software interface
  #include <SoftwareSerial.h>
  #define ESP32CAMERA_TX_PIN 1// TX pin
  #define ESP32CAMERA_RX_PIN 1// TX pin
#endif

#define ESP32CAMERA_KEY "fotografou"// JSON picture has been taken key
#define ESP32CAMERA_REQUEST_ID 1// Command to request if a picture was successfully taken in the last attempt
#define ESP32CAMERA_PHOTOGRAPH_ID 2// Command to take a picture

class ESP32Camera : public Component{
    private:
        bool esp32camera_data;
        #if defined(ESP32) || defined(ESP8266)// For ESP
            HardwareSerial* camSerial;
        #else// For Arduino
            SoftwareSerial* camSerial;
        #endif

    public:
        ESP32Camera();// Create object
        ~ESP32Camera();// Release memory
        void gatherData();// Get data from component
        void printData();// Display data for test
        void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload);// Create JSON entries
        void saveCSVToFile(SdFile* my_file);// Save data to MicroSD card
        void takePicture();// Request to take a picture
        void start() override;
};