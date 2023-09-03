/*
ms5611.h
Library for barometric pressure sensor MS5611-01BA on I2C with arduino

by Petr Gronat@2014
*/

// Include guard token - prevents to include header file twice
#ifndef MS5611_h
#define MS5611_h 	//create token

// Include Arduino libraries
#include <Arduino.h>
#include <Wire.h>
#include <math.h>

#define N_PROM_PARAMS 6

// address of the device MS5611
#define ADD_MS5611 0x77// can be 0x76 if CSB pin is connected to GND

#define OSR 					2		// 0-3
#define CMD_RESET 				0x1E
#define CMD_ADC_READ			0x00
#define CMD_CONV_D1_BASE 		0x40
#define CMD_CONV_D2_BASE 		0x50
#define CONV_REG_SIZE 			0x02
#define CMD_PROM_READ_BASE		0xA2
#define PROM_REG_SIZE			0x02
#define NBYTES_CONV 			3
#define NBYTES_PROM 			2

// Temperature sampling period threshold [milliseconds]
// Kindly read the comment bellow in getPressure() method
#define T_THR					1000

class MS5611{
	public:
		MS5611();//constructor
		void 		setPressureOffset(int16_t _pressureOffset);
		void 		begin();
		uint32_t 	getRawTemperature();
		int32_t		getTemperature();
		uint32_t 	getRawPressure();
		int32_t 	getPressure();
		double 		getAltitude();
		void 		readCalibration();
		void 		getCalibration(uint16_t *);
		void 		sendCommand(uint8_t);
		uint32_t 	readnBytes(uint8_t);

	private:
		void 		reset();
		//variables
		TwoWire *_wire;
		int16_t		pressureOffset;
		int32_t 	_PR;
		int32_t  	_TEMP;
		int32_t 	_dT;
		uint16_t 	_CA[N_PROM_PARAMS];
		uint32_t 	_lastTime;
		int64_t		OFF2;
		int64_t		SENS2;
};

#endif