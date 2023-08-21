#include "pch.h"
#include "MicroSDReaderWriter.h"

MicroSDReaderWriter::MicroSDReaderWriter(const char* _datafileName){// Create object
    sd = new SdFat();
    my_file = new SdFile();
    strcpy(datafileName, _datafileName);// Copy filename (date and hour)
    strcat(datafileName, FILE_EXTENSION);// Append file extension
    while(!sd->begin(SD_CS_PIN, SPI_FULL_SPEED)){
        Serial.println(F("Waiting for MicroSD."));
        delay(CALIBRATION_DELAY);
    }
}

MicroSDReaderWriter::~MicroSDReaderWriter(){// Release memory
    delete sd;
    delete my_file;
}

SdFile* MicroSDReaderWriter::gatherData(){// Get data from component
    if(my_file->open(datafileName, O_RDWR | O_CREAT | O_AT_END))
        return my_file;// Open file or create if not exists for read/write at the end
    else{
        Serial.println(F("MicroSD card failed."));
        return nullptr;
    }
}