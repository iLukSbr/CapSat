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

#include "pch.h"
#include "Rainmeter.h"

Rainmeter::Rainmeter():
    rainmeter_data(0)
{// Create object
    pinMode(RAINMETER_PIN, INPUT);// Defines RAINMETER_PIN as input
    start();
}

Rainmeter::~Rainmeter(){// Release memory

}

void Rainmeter::gatherData(){// Get data from component
    multiPrintln(F("Gathering rainmeter MH-RD data..."));
    digitalRead(RAINMETER_PIN) == HIGH ? rainmeter_data=0 : rainmeter_data=1;// HIGH, 0 = dry; LOW, 1 = wet
}

void Rainmeter::printData(){// Display data for test
    multiPrint(F("Rainmeter MH-RD: "));
    multiPrint(rainmeter_data);
    multiPrintln();
}

void Rainmeter::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    payload[F(RAIN_KEY)] = rainmeter_data;
}

void Rainmeter::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(rainmeter_data);
    my_file->print(F(","));
}

void Rainmeter::start(){
    multiPrintln(F("Starting rainmeter MH-RD..."));
    started = true;
    multiPrintln(F("Rainmeter MH-RD OK!"));
}