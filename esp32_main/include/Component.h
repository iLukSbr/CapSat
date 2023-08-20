#pragma once

#include <MicroSDReaderWriter.h>
#include <ArduinoJson.h>

// General
class Component{
  public:
    Component(){}
    virtual ~Component(){}
    virtual void gatherData() = 0;
    virtual void printData() = 0;
    virtual void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) = 0;
    virtual void saveCSVToFile(SdFile* my_file) = 0;
};