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
#include "MicroSDReaderWriter.h"

MicroSDReaderWriter::MicroSDReaderWriter(const char* _datafileName):
    sd(new SdFat()),// MicroSD card
    my_file(new SdFile())// File
{// Create object
    multiPrintln(F("Starting MicroSD..."));
    strcpy(datafileName, _datafileName);// Copy filename (date and hour)
    strcat(datafileName, FILE_EXTENSION);// Append file extension
    multiPrint(F("MicroSD filename: "));
    multiPrintln(datafileName);
    while(!sd->begin(SD_CS_PIN, SPI_SIXTEENTH_SPEED)){
        multiPrintln(F("Waiting for MicroSD... Try reinserting."));
        delay(CALIBRATION_DELAY);
    }
    multiPrintln(F("MicroSD OK!"));
}

MicroSDReaderWriter::~MicroSDReaderWriter(){// Release memory
    delete sd;
    delete my_file;
}

SdFile* MicroSDReaderWriter::gatherData(){// Get data from component
    multiPrintln(F("Gathering MicroSD data..."));
    if(my_file->open(datafileName, O_RDWR | O_CREAT | O_AT_END))
        return my_file;// Open file or create if not exists for read/write at the end
    else{
        multiPrintln(F("MicroSD card failed."));
        return nullptr;
    }
}