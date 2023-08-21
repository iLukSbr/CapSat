#pragma once

#define CALIBRATION_DELAY 1000// Delay between calibration requests (ms)
#define SERIAL_BAUD_RATE 230400// Serial baud rate
#if defined(ESP32) || defined(ESP8266)
  #define PIN_MAX_VOLTAGE 3.3
  #define PIN_MAX_ANALOG 4095.f
#else
  #define PIN_MAX_VOLTAGE 5.f
  #define PIN_MAX_ANALOG 1023.f
#endif