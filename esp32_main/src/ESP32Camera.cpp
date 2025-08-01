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

#include "settings.h"
#include "ESP32Camera.h"

ESP32Camera::ESP32Camera():
    esp32camera_data(false),
    #if defined(ESP32) || defined(ESP8266)// For ESP
        camSerial(new HardwareSerial(UART_NUM_1))
    #else// For Arduino
        camSerial(new SoftwareSerial(ESP32CAMERA_RX_PIN, ESP32CAMERA_TX_PIN))
    #endif
{
    camSerial->begin(SERIAL_BAUD_RATE);
    start();
}

ESP32Camera::~ESP32Camera(){
    delete camSerial;
}

void ESP32Camera::gatherData(){// Get data from component
    multiPrintln(F("Gathering camera ESP32-CAM data..."));
    camSerial->println(ESP32CAMERA_REQUEST_ID);
    if(camSerial->available())// If data was received
        camSerial->read()==1 ? esp32camera_data=true : esp32camera_data=false;// True if a picture was successfully taken in the last attempt
}

void ESP32Camera::printData(){// Display data for test
    multiPrint("Camera ESP32-CAM: ");
    multiPrint(esp32camera_data);
    multiPrintln();
}

void ESP32Camera::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(ESP32CAMERA_KEY)] = esp32camera_data;
}

void ESP32Camera::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(esp32camera_data ? 0 : 1);
    my_file->print(F(","));
}

void ESP32Camera::start(){
    multiPrintln(F("Starting ESP32-CAM camera..."));
    started = true;
    multiPrintln(F("Camera ESP32-CAM OK!"));
}

void ESP32Camera::takePicture(){// Take a picture
    camSerial->println(ESP32CAMERA_PHOTOGRAPH_ID);
}