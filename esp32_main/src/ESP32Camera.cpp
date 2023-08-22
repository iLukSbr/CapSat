#include "pch.h"
#include "ESP32Camera.h"

ESP32Camera::ESP32Camera(){
    #if defined(ESP32) || defined(ESP8266)// For ESP
        camSerial = new HardwareSerial(UART_NUM_2);
    #else// For Arduino
        camSerial = new SoftwareSerial(ESP32CAMERA_RX_PIN, ESP32CAMERA_TX_PIN);
    #endif
    camSerial->begin(SERIAL_BAUD_RATE);
}

ESP32Camera::~ESP32Camera(){
    delete camSerial;
}

void ESP32Camera::gatherData(){// Get data from component
    camSerial->println(ESP32CAMERA_REQUEST_ID);
    while(camSerial->available())// If data was received
        camSerial->read()==1 ? esp32camera_data=true : esp32camera_data=false;// True if a picture was successfully taken in the last attempt
}

void ESP32Camera::printData(){// Display data for test
    Serial.print("ESP32-CAM: ");
    Serial.print(esp32camera_data);
    Serial.println();
}

void ESP32Camera::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(ESP32CAMERA_KEY)] = esp32camera_data;
}

void ESP32Camera::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(esp32camera_data ? 0 : 1);
    my_file->print(F(","));
}

void ESP32Camera::takePicture(){// Take a picture
    camSerial->println(ESP32CAMERA_PHOTOGRAPH_ID);
}