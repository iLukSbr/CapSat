#include "pch.h"
#include "ESP32_camera.h"

ESP32Camera::ESP32Camera(){
    
}

ESP32Camera::~ESP32Camera{
    
}

void ESP32Camera::gatherData(){// Get data from component

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