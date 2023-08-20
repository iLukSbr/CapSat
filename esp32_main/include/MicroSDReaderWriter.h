#pragma once

#include "pch.h"

// MicroSD reader/writer
// https://github.com/greiman/SdFat
#include <SdFat.h>

#define JSON_FORMAT// Save as JSON_FORMAT or CSV_FORMAT

#ifdef JSON_FORMAT
  #define FILE_EXTENSION ".json"// Saving file extension
#elif defined(CSV_FORMAT)
  #define FILE_EXTENSION ".csv"
#else
  #define FILE_EXTENSION ".txt"
#endif

#define SD_CS_PIN 10// Chip select pin for MicroSD card
#define DATAFILE_NAME_LENGTH 25// Datafile name characters length
#define MICROSD_CALIBRATION_DELAY 1000// Delay between calibration requests (ms)

class MicroSDReaderWriter{
  private:
    SdFat* sd = nullptr;// MicroSD card
    SdFile* my_file = nullptr;// File
    char datafileName[DATAFILE_NAME_LENGTH];// Filename
  public:
    MicroSDReaderWriter(const char* _datafileName){
      sd = new SdFat();
      my_file = new SdFile();
      strcpy(datafileName, _datafileName);// Copy filename (date and hour)
      strcat(datafileName, FILE_EXTENSION);// Append file extension
      while(!sd->begin(SD_CS_PIN, SPI_FULL_SPEED)){
        Serial.println(F("Waiting for MicroSD."));
        delay(MICROSD_CALIBRATION_DELAY);
      }
    }
    ~MicroSDReaderWriter(){
      delete sd;
      delete my_file;
    }
    SdFile* gatherData(){
      if(my_file->open(datafileName, O_RDWR | O_CREAT | O_AT_END))
        return my_file;// Open file or create if not exists for read/write at the end
      else{
        Serial.println(F("MicroSD card failed."));
        return nullptr;
      }
    }
};