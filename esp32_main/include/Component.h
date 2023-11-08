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

#pragma once

#include <ArduinoJson.h>

#include "MicroSDReaderWriter.h"
#include "Message.h"

// General
class Component : public Message{
  protected:
    bool started;
    unsigned long stopwatch;
    unsigned long reading_delay;

  public:
    Component();// Create object
    virtual ~Component();// Release memory
    virtual void gatherData() = 0;// Get data from component
    virtual void printData() = 0;// Display data for test
    virtual void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) = 0;// Create JSON entries
    virtual void saveCSVToFile(SdFile* my_file) = 0;// Save data to MicroSD card
    virtual void start() = 0;// Start component
    bool isInTime() const;// Check if it is time to read the sensor
    bool isStarted() const;// Check if the component was started properly
    float round3(const float value);// Round value to 3 decimal places
};