#pragma once

#include "MicroSDReaderWriter.h"

// General
class Component{
  public:
    virtual void begin() = 0;
    virtual void gatherData() = 0;
    virtual void printData() = 0;
    virtual void saveData(SdFile* my_file) = 0;
};