#pragma once

#include <Arduino.h>

// ADS1115 ADC 16 bits
// https://github.com/wollewald/ADS1115_WE
#include <ADS1115_WE.h>

typedef enum{
	CH_CO,
	CH_NO2,
	CH_NH3
}channel_t;

typedef enum{
	CO,
	NO2,
	NH3,
	C3H8,
	C4H10,
	CH4,
	H2,
	C2H5OH
}gas_t;

class MICS6814{
	public:
		MICS6814(ADS1115_MUX pinCO, ADS1115_MUX pinNO2, ADS1115_MUX pinNH3,  ADS1115_WE* _adc);

		void calibrate();
		void loadCalibrationData(uint16_t base_NH3, uint16_t base_RED, uint16_t base_OX);

		double measure(gas_t gas);
		double getCurrentRatio(channel_t channel) const;

		uint16_t getResistance(channel_t channel) const;
		uint16_t getBaseResistance(channel_t channel) const;

	private:
		ADS1115_WE* adc;

		ADS1115_MUX _pinCO;
		ADS1115_MUX _pinNO2;
		ADS1115_MUX _pinNH3;

		uint16_t _baseNH3;
		uint16_t _baseCO;
		uint16_t _baseNO2;
		uint16_t maxVoltage = 3300;// mV
};