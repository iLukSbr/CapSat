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

// MicroSD reader/writer using Kodak MicroSDHC 32 Gb Class 10 U1
// https://github.com/greiman/SdFat
#include <SdFat.h>

#include "Message.h"

#ifndef MICROSD_READER_WRITER
  #define MICROSD_READER_WRITER 1
#endif

#define JSON_FORMAT// Save as .json
// #define CSV_FORMAT// Save as .csv

#ifdef JSON_FORMAT
  #define FILE_EXTENSION ".json"// Saving file extension
#elif defined(CSV_FORMAT)
  #define FILE_EXTENSION ".csv"
#else
  #define FILE_EXTENSION ".txt"
#endif

#define SD_CS_PIN 5// Chip select pin for MicroSD card
#define SD_FREQ 9600// MicroSD SPI card frequency
#define DATAFILE_NAME_LENGTH 25// Datafile name characters length

class MicroSDReaderWriter : public Message{
  private:
    bool started;
    SdFat* sd;// MicroSD card
    SdFile* my_file;// File
    char datafileName[DATAFILE_NAME_LENGTH];// Filename
    
  public:
    MicroSDReaderWriter(const char* _datafileName);// Create object
    ~MicroSDReaderWriter();// Release memory
    bool isStarted() const;
    SdFile* gatherData();// Get data from component
    void start();
};