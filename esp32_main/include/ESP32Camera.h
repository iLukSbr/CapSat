#include "Component.h"

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
        bool esp32camera_data = false;
        #if defined(ESP32) || defined(ESP8266)// For ESP
            HardwareSerial* camSerial = nullptr;
        #else// For Arduino
            SoftwareSerial* camSerial = nullptr;
        #endif

    public:
        ESP32Camera();// Create object
        ~ESP32Camera();// Release memory
        void gatherData();// Get data from component
        void printData();// Display data for test
        void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload);// Create JSON entries
        void saveCSVToFile(SdFile* my_file);// Save data to MicroSD card
        void takePicture();// Request to take a picture
};