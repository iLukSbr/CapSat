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

#include "Component.h"

// I²C interface
#include <Wire.h>

// GY-511 LSM303 magnetometer
// https://github.com/pololu/lsm303-arduino
#include <LSM303.h>

#ifndef MAGNETOMETER
  #define MAGNETOMETER 1
#endif

#define MAGNETOMETER_KEY "magnetometro"// JSON magnetometer key
#define MAGNETOMETER_DIRECTION_SIZE 4// Direction string length
#define MAGNETOMETER_HEADING_KEY "angulo"// JSON compass heading key
#define MAGNETOMETER_DIRECTION_KEY "direcao"// JSON compass direction key
#define MAGNETOMETER_CALIBRATION_DURATION 10000// (ms) Time needed to calibrate

class Magnetometer : public Component{
  private:
        LSM303* compass;

        char magnetometer_direction[4] = {0};

        float magnetometer_heading;

    protected:
        const char directions[16][3] = {
            {' ', ' ', 'N'},
            {'N', 'N', 'E'},
            {' ', 'N', 'E'},
            {'E', 'N', 'E'},
            {' ', ' ', 'E'},
            {'E', 'S', 'E'},
            {' ', 'S', 'E'},
            {'S', 'S', 'E'},
            {' ', ' ', 'S'},
            {'S', 'S', 'W'},
            {' ', 'S', 'W'},
            {'W', 'S', 'W'},
            {' ', ' ', 'W'},
            {'W', 'N', 'W'},
            {' ', 'N', 'W'},
            {'N', 'N', 'W'},
	    };

  public:
    Magnetometer();// Create object
    ~Magnetometer();// Release memory

    void gatherData() override;// Get data from component
    void printData() override;// Display data for test
    void makeJSON(const bool& isHTTP, JsonDocument& doc, JsonObject& payload) override;// Create JSON entries
    void saveCSVToFile(SdFile* my_file) override;// Save data to MicroSD card
    void start() override;
};