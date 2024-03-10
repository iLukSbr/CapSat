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
#include "Magnetometer.h"

Magnetometer::Magnetometer():
    compass(new LSM303()),// Instantiate compass
    magnetometer_heading(0.f)// Initialize heading
{// Create object
    multiPrintln(F("Starting magnetometer GY-511..."));
    start();
}

Magnetometer::~Magnetometer(){// Release memory
    delete compass;
}

void Magnetometer::gatherData(){// Get data from component
    multiPrintln(F("Gathering magnetometer GY-511 data..."));
    compass->read();
    magnetometer_heading = compass->heading();
    unsigned long a = (magnetometer_heading>-0.5 ? magnetometer_heading/22.5 : (magnetometer_heading+360)/22.5);
	unsigned long r = a - (int)a;
	byte part = (r>=.5 ? ceil(a) : floor(a));
	magnetometer_direction[0] = directions[part][0];
	magnetometer_direction[1] = directions[part][1];
	magnetometer_direction[2] = directions[part][2];
    magnetometer_direction[3] = '\0';
}

void Magnetometer::printData(){// Display data for test
    multiPrint(F("Magnetometer GY-511: "));
    multiPrint(magnetometer_heading);
    multiPrint(F("° "));
    multiPrintln(magnetometer_direction);
}

void Magnetometer::makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload){// Create JSON entries
    if(!isHTTP)
        payload[F(MAGNETOMETER_HEADING_KEY)] = magnetometer_heading;
    payload[F(MAGNETOMETER_DIRECTION_KEY)] = magnetometer_direction;
}

void Magnetometer::saveCSVToFile(SdFile* my_file){// Save data to MicroSD card
    my_file->print(magnetometer_heading);
    my_file->print(F(","));
    my_file->print(magnetometer_direction);
    my_file->print(F(","));
}

void Magnetometer::start(){
    LSM303::vector<int16_t> running_min = {-257, -349, -193}, running_max = {241, 172, 257};
    compass->init();
    compass->enableDefault();
	unsigned long startTime = millis();
    Serial.println(F("Calibrating GY-511 magnetometer..."));
	while((millis() - startTime) < MAGNETOMETER_CALIBRATION_DURATION){
        compass->read();
        running_min.x = min(running_min.x, compass->m.x);
        running_min.y = min(running_min.y, compass->m.y);
        running_min.z = min(running_min.z, compass->m.z);
        running_max.x = max(running_max.x, compass->m.x);
        running_max.y = max(running_max.y, compass->m.y);
        running_max.z = max(running_max.z, compass->m.z);
    }
    compass->m_min = running_min;
    compass->m_max = running_max;
    started = true;
    multiPrintln(F("Magnetometer GY-511 OK!"));
}