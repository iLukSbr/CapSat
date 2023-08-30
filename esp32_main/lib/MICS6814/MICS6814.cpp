#include "MICS6814.h"

MICS6814::MICS6814(ADS1115_MUX pinCO, ADS1115_MUX pinNO2, ADS1115_MUX pinNH3, ADS1115_WE* _adc):
	_pinCO(pinCO),
	_pinNO2(pinNO2),
	_pinNH3(pinNH3),
	adc(_adc)
{
    while(!adc->init())
        Serial.println(F("Waiting for ADC..."));
    adc->setMeasureMode(ADS1115_CONTINUOUS);
	#if defined(ESP32) || defined(ESP8266)
		adc->setVoltageRange_mV(ADS1115_RANGE_4096);
	#else
		maxVoltage = 5000;
		adc->setVoltageRange_mV(ADS1115_RANGE_6144);
	#endif
}

/*Calibrates MICS-6814 before use

Work algorithm:
Continuously measures resistance,
with saving the last N measurements in the buffer.
Calculates the floating average of the last seconds.
If the current measurement is close to average,
we believe that the calibration was successful.*/
void MICS6814::calibrate(){
	// The number of seconds that must pass before
	// than we will assume that the calibration is complete
	// (Less than 64 seconds to avoid overflow)
	uint8_t seconds = 10;
	uint16_t iter = 0;

	// Tolerance for the average of the current value
	uint8_t delta = 10;

	// Measurement buffers
	uint16_t bufferNH3[seconds];
	uint16_t bufferCO[seconds];
	uint16_t bufferNO2[seconds];

	// Pointers for the next item in the buffer
	uint8_t pntrNH3 = 0;
	uint8_t pntrCO = 0;
	uint8_t pntrNO2 = 0;

	// The current floating amount in the buffer
	uint16_t fltSumNH3 = 0;
	uint16_t fltSumCO = 0;
	uint16_t fltSumNO2 = 0;

	// Current measurement
	uint16_t curNH3;
	uint16_t curCO;
	uint16_t curNO2;

	// Flag of stability of indications
	bool isStableNH3 = false;
	bool isStableCO = false;
	bool isStableNO2 = false;

	// We kill the buffer with zeros
	for (int i=0; i<seconds; i++){
		bufferNH3[i] = 0;
		bufferCO[i] = 0;
		bufferNO2[i] = 0;
	}

	// Calibrate
	//do{
		delay(1000);
		unsigned long rs = 0;
		delay(50);
		for (int i=0; i<3; i++){
			delay(1);
			adc->setCompareChannels(_pinNH3);
			rs += map((long)round(adc->getResult_mV()), 0, maxVoltage, 0, 1023);
		}
		curNH3 = rs/3;
		rs = 0;
		delay(50);
		for (int i=0; i<3; i++){
			delay(1);
			adc->setCompareChannels(_pinCO);
			rs += map((long)round(adc->getResult_mV()), 0, maxVoltage, 0, 1023);
		}
		curCO = rs/3;
		rs = 0;
		delay(50);
		for (int i=0; i<3; i++){
			delay(1);
			adc->setCompareChannels(_pinNO2);
			rs += map((long)round(adc->getResult_mV()), 0, maxVoltage, 0, 1023);
		}
		curNO2 = rs/3;

		// Update the floating amount by subtracting the value, 
		// to be overwritten, and adding a new value.
		fltSumNH3 = fltSumNH3 + curNH3 - bufferNH3[pntrNH3];
		fltSumCO = fltSumCO + curCO - bufferCO[pntrCO];
		fltSumNO2 = fltSumNO2 + curNO2 - bufferNO2[pntrNO2];

		// Store d buffer new values
		bufferNH3[pntrNH3] = curNH3;
		bufferCO[pntrCO] = curCO;
		bufferNO2[pntrNO2] = curNO2;

		// Define flag states
		isStableNH3 = (abs(fltSumNH3/seconds - curNH3) < delta);
		isStableCO = (abs(fltSumCO/seconds - curCO) < delta);
		isStableNO2 = (abs(fltSumNO2/seconds - curNO2) < delta);

		// Pointer to a buffer
		pntrNH3 = (pntrNH3 + 1)%seconds;
		pntrCO = (pntrCO + 1)%seconds;
		pntrNO2 = (pntrNO2 + 1)%seconds;

		iter++;
	//}while((!isStableNH3 || !isStableCO || !isStableNO2) && iter < 3);

	_baseNH3 = fltSumNH3/seconds;
	_baseCO = fltSumCO/seconds;
	_baseNO2 = fltSumNO2/seconds;
}

void MICS6814::loadCalibrationData(uint16_t baseNH3, uint16_t baseCO, uint16_t baseNO2){
	_baseNH3 = baseNH3;
	_baseCO = baseCO;
	_baseNO2 = baseNO2;
}

/*Measures the gas concentration in ppm for the specified gas.

@param gas
Gas ​​for concentration calculation.

@return
Current gas concentration in parts per million (ppm).*/
double MICS6814::measure(gas_t gas){
	double ratio0=getCurrentRatio(CH_NH3), ratio1=getCurrentRatio(CH_CO), ratio2=getCurrentRatio(CH_NO2), c=0;
	switch(gas){
		case CO:
			c = pow(ratio1, -1.179)*4.385;
			break;
		case NO2:
			c = pow(ratio2, 1.007)/6.855;
			break;
		case NH3:
			c = pow(ratio0, -1.67)/1.47;
			break;
		case C3H8:
			c = pow(ratio0, -2.518)*570.164;
			break;
		case C4H10:
			c = pow(ratio0, -2.138)*398.107;
			break;
		case CH4:
			c = pow(ratio1, -4.363)*630.957;
			break;
		case H2:
			c = pow(ratio1, -1.8)*0.73;
			break;
		case C2H5OH:
			c = pow(ratio1, -1.552)*1.622;
			break;
		default:
			c = -1;
			break;
	}
	return isnan(c) ? -1 : c;
}

/*Requests the current resistance for this channel from the sensor. 
Value is the value of the ADC in the range from 0 to 1024.

@param channel
Channel for reading base resistance.

@return
Unsigned 16-bit base resistance of the selected channel.*/
uint16_t MICS6814::getResistance(channel_t channel) const{
	unsigned long rs = 0;
	int counter = 0;
	switch (channel){
		case CH_CO:
			for (int i=0; i<100; i++){
				adc->setCompareChannels(_pinCO);
				rs += map((long)round(adc->getResult_mV()), 0, maxVoltage, 0, 1023);
				counter++;
				delay(2);
			}
			break;
		case CH_NO2:
			for (int i=0; i<100; i++){
				adc->setCompareChannels(_pinNO2);
				rs += map((long)round(adc->getResult_mV()), 0, maxVoltage, 0, 1023);
				counter++;
				delay(2);
			}
			break;
		case CH_NH3:
			for (int i=0; i<100; i++){
				adc->setCompareChannels(_pinNO2);
				rs += map((long)round(adc->getResult_mV()), 0, maxVoltage, 0, 1023);
				counter++;
				delay(2);
			}
			break;
		default:
			break;
	}
	return counter!=0 ? rs/counter : 0;
}

uint16_t MICS6814::getBaseResistance(channel_t channel) const{
	switch(channel){
		case CH_NH3:
			return _baseNH3;
			break;
		case CH_CO:
			return _baseCO;
			break;
		case CH_NO2:
			return _baseNO2;
			break;
		default:
			return 0;
			break;
	}
}

/*Calculates the current resistance coefficient for a given channel.

@param channel
Channel for requesting resistance values.

@return
Floating point resistance coefficient for this sensor.*/
double MICS6814::getCurrentRatio(channel_t channel) const{
	double baseResistance=(double)getBaseResistance(channel), resistance=(double)getResistance(channel);
	return resistance/baseResistance*(1023.0 - baseResistance)/(1023.0 - resistance);
}