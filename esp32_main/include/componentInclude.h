/* Comment .h to disable device or uncomment to enable */

#pragma once

/* === UART Interface === */
#include "ParticulateMeter.h"// PMSA003 particulate meter
#include "Gps.h"// NEO-M8N GPS
// #include "ESP32Camera.h"// ESP32-CAM

/* === I²C Interface === */
#include "Accelerometer.h"// MPU-9250 accelerometer/gyroscope
#include "Altimeter.h"// MS5611 altimeter/barometer
#include "Magnetometer.h"// QMC5883L magnetometer
#include "Humidimeter.h"// ENS160+AHT21 carbodioximeter, TVOC meter and humidimeter
#include "Multimeter.h"// INA219 multimeter
#include "RTClock.h"// DS3231 RTC

/* === Analog Interface === */
#include "GasMeter.h"// MiCS-6814 gas meter
#include "Ozonoscope.h"// MQ-131 ozonoscope
// #include "Thermometer.h"// NTC thermometer
// #include "Rainmeter.h"// Rain sensor
// #include "UVRadiometer.h"// Taidacent UV level sensor

#ifndef ACCELEROMETER
    #define ACCELEROMETER 0
    #define _ACCELEROMETER
#endif
#ifndef ALTIMETER
    #define ALTIMETER 0
    #define _ALTIMETER
#endif
#ifndef ESP32_CAMERA
    #define ESP32_CAMERA 0
    #define _ESP32_CAMERA
#endif
#ifndef GAS_METER
    #define GAS_METER 0
    #define _GAS_METER
#endif
#ifndef GPS
    #define GPS 0
    #define _GPS
#endif
#ifndef HUMIDIMETER
    #define HUMIDIMETER 0
    #define _HUMIDIMETER
#endif
#ifndef MAGNETOMETER
    #define MAGNETOMETER 0
    #define _MAGNETOMETER
#endif
#ifndef MICROSD_READER_WRITER
    #define MICROSD_READER_WRITER 0
    #define _MICROSD_READER_WRITER
#endif
#ifndef MULTIMETER
    #define MULTIMETER 0
    #define _MULTIMETER
#endif
#ifndef OZONOSCOPE
    #define OZONOSCOPE 0
    #define _OZONOSCOPE
#endif
#ifndef PARTICULATE_METER
    #define PARTICULATE_METER 0
    #define _PARTICULATE_METER
#endif
#ifndef RAINMETER
    #define RAINMETER 0
    #define _RAINMETER
#endif
#ifndef RTCLOCK
    #define RTCLOCK 0
    #define _RTCLOCK
#endif
#ifndef THERMOMETER
    #define THERMOMETER 0
    #define _THERMOMETER
#endif
#ifndef UV_RADIOMETER
    #define UV_RADIOMETER 0
    #define _UV_RADIOMETER
#endif
#ifndef RELAY
    #define RELAY 0
    #define _RELAY
#endif

// Components quantity inside vector
#define COMPONENTS_VECTOR_SIZE ACCELEROMETER+ALTIMETER+ESP32_CAMERA+GAS_METER+GPS+HUMIDIMETER+MAGNETOMETER+MULTIMETER+OZONOSCOPE+PARTICULATE_METER+RAINMETER+RTCLOCK+THERMOMETER+UV_RADIOMETER