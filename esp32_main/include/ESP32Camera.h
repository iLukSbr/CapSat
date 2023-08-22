#include "Component.h"

class ESP32Camera : public Component{
    private:
        bool esp32camera_data = false;

    public:
        ESP32Camera();// Create object
        ~ESP32Camera();// Release memory
        virtual void gatherData();// Get data from component
        virtual void printData();// Display data for test
        virtual void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload);// Create JSON entries
        virtual void saveCSVToFile(SdFile* my_file);// Save data to MicroSD card
}