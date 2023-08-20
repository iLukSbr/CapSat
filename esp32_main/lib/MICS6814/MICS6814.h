#pragma once

#include <Arduino.h>

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
		MICS6814(int pinCO, int pinNO2, int pinNH3);

		void calibrate();
		void loadCalibrationData(uint16_t base_NH3, uint16_t base_RED, uint16_t base_OX);

		double measure(gas_t gas);
		double getCurrentRatio(channel_t channel) const;

		uint16_t getResistance(channel_t channel) const;
		uint16_t getBaseResistance(channel_t channel) const;

	private:
		uint8_t _pinCO;
		uint8_t _pinNO2;
		uint8_t _pinNH3;

		uint16_t _baseNH3;
		uint16_t _baseCO;
		uint16_t _baseNO2;
};