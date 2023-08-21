#pragma once

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

class MicroSDReaderWriter{
  private:
    SdFat* sd = nullptr;// MicroSD card
    SdFile* my_file = nullptr;// File
    char datafileName[DATAFILE_NAME_LENGTH];// Filename
    
  public:
    MicroSDReaderWriter(const char* _datafileName);// Create object
    ~MicroSDReaderWriter();// Release memory
    SdFile* gatherData();// Get data from component
};