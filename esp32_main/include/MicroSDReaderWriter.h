#pragma once

#include "pch.h"

// MicroSD reader/writer
// https://github.com/greiman/SdFat
#include <SdFat.h>

#define SD_CS_PIN 10// Chip select pin for MicroSD card
#define DATAFILE_NAME_LENGTH 25// Datafile name characters length
#define FILE_EXTENSION ".csv"// Saving file extension
#define MICROSD_CALIBRATION_DELAY 1000// Delay between calibration requests (ms)

class MicroSDReaderWriter{
  private:
    SdFat sd;// MicroSD card
    SdFile my_file;// File
    char datafileName[DATAFILE_NAME_LENGTH];// Filename
  public:
    void begin(char* _datafileName){
      strcpy(datafileName, _datafileName);// Copy filename (date and hour)
      strcat(datafileName, FILE_EXTENSION);// Append file extension
      while(!sd.begin(SD_CS_PIN, SPI_FULL_SPEED)){
        Serial.println(F("Waiting for MicroSD."));
        delay(MICROSD_CALIBRATION_DELAY);
      }
    }
    SdFile* gatherData(){
      if(my_file.open(datafileName, O_RDWR | O_CREAT | O_AT_END))
        return &my_file;// Open file or create if not exists for read/write at the end
      else{
        Serial.println(F("MicroSD card failed."));
        return nullptr;
      }
    }
};