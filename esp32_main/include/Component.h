#pragma once

#include <MicroSDReaderWriter.h>
#include <ArduinoJson.h>

// General
class Component{
  public:
    Component();// Create object
    virtual ~Component();// Release memory
    virtual void gatherData() = 0;// Get data from component
    virtual void printData() = 0;// Display data for test
    virtual void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) = 0;// Create JSON entries
    virtual void saveCSVToFile(SdFile* my_file) = 0;// Save data to MicroSD card
};