/*
MS5611.h
Library for barometric pressure sensor MS5611-01BA on I2C with arduino

by Petr Gronat@2014
*/
#include "MS5611.h"

MS5611::MS5611():
	_wire(&Wire),
	pressureOffset(0),
	_PR(0),
	_TEMP(0),
	_lastTime(T_THR)
{
	for(uint8_t k=0; k<N_PROM_PARAMS; k++) 
		_CA[k]=69;
}

void MS5611::begin(){
	reset();
	delay(100);
	readCalibration();
}

int32_t	MS5611::getPressure(){
	getTemperature();// updates temperature _dT and _TEMP
	uint32_t D1 = getRawPressure();
	
	int64_t OFF  = (int64_t)_CA[2-1]*65536 
				 + (int64_t)_CA[4-1]*_dT/128;
	OFF -= OFF2;

	int64_t SENS = (int64_t)_CA[1-1]*32768 
				 + (int64_t)_CA[3-1]*_dT/256;
	SENS -= SENS2;

	_PR = (D1*SENS/2097152 - OFF)/16384 + pressureOffset;
	return _PR;
}

uint32_t MS5611::getRawPressure(){
	sendCommand(CMD_CONV_D1_BASE+OSR*CONV_REG_SIZE);// read sensor, prepare a data
	delay(1+2*OSR);// wait at least 8.33us for full oversampling
	sendCommand(CMD_ADC_READ);// get ready for reading the data
	return readnBytes(NBYTES_CONV);// reading the data
}

int32_t MS5611::getTemperature(){
	// Code below can be uncommented for slight speedup:
	// NOTE: Be sure what you do! Notice that Delta 1C ~= Delta 2hPa
	//****************
	// if(abs(millis()-_lastTime)<T_THR)
	// 	return _TEMP;
	//_lastTime = millis();
	//****************
	int64_t T2;
	uint32_t D2; 	
	D2  = getRawTemperature();
	_dT = D2-((uint32_t)_CA[5-1] * 256);// update '_dT'
	// Below, 'dT' and '_CA[6-1]'' must be casted in order to prevent overflow
	// A bitwise division can not be dobe since it is unpredictible for signed integers
	_TEMP = 2000 + ((int64_t)_dT * _CA[6-1])/8388608;
	if(_TEMP < 2000){
		T2 = ((int64_t)_dT*_dT)/2147483648;
		OFF2 = 5*(int64_t)_TEMP*_TEMP/2 - 10000*(int64_t)_TEMP + 10000000;
		SENS2 = OFF2/2;
		if(_TEMP < -1500){
			OFF2 += 7*(int64_t)_TEMP*_TEMP + 21000*(int64_t)_TEMP + 15750000;
			SENS2 += 11*(int64_t)_TEMP*_TEMP/2 + 16500*(int64_t)_TEMP + 12375000;
		}
	}
	else{
		T2 = 0;
		OFF2 = 0;
		SENS2 = 0;
	}
	_TEMP -= T2;
	return _TEMP;
}

uint32_t MS5611::getRawTemperature(){	
	sendCommand(CMD_CONV_D2_BASE+OSR*CONV_REG_SIZE);// read sensor, prepare a data
	delay(1+2*OSR);// wait at least 8.33us
	sendCommand(CMD_ADC_READ);// get ready for reading the data
	return readnBytes(NBYTES_CONV);// reading the data
}

void MS5611::readCalibration(){
	for(uint8_t k=0;k<6;k++){
		sendCommand(CMD_PROM_READ_BASE + k*2);
		_CA[k] = (uint16_t) (readnBytes(NBYTES_PROM) & 0xFFFF);// masking out two LSB
	}
}

void MS5611::getCalibration(uint16_t *CA){
	for(uint8_t k=0;k<N_PROM_PARAMS;k++)
		CA[k] = _CA[k];
}

void MS5611::sendCommand(uint8_t cmd){
	_wire->beginTransmission(ADD_MS5611);
	_wire->write(cmd);
	_wire->endTransmission();
}

uint32_t MS5611::readnBytes(uint8_t nBytes){
	if (0<nBytes && nBytes<5){	
		_wire->beginTransmission(ADD_MS5611);
		_wire->requestFrom((uint8_t)ADD_MS5611, nBytes);
			uint32_t data = 0;
			if(_wire->available()!=nBytes){
				_wire->endTransmission();
				return 0;
			}
			for (int8_t k=nBytes-1; k>=0; k--)
				data |= ( (uint32_t) _wire->read() << (8*k) );// concantenate bytes
		_wire->endTransmission();
		return data;
	}// too many bytes or
	return 0;// no byte required
}

void MS5611::reset(){
	_wire->beginTransmission(ADD_MS5611);
	_wire->write(CMD_RESET);
	_wire->endTransmission();
}

double MS5611::getAltitude(){
  	double altitude = 44307.692307692 - 4943.777885853*pow((double)getPressure(), 0.1902665);
  	if(altitude >= 11000.0)
    	return 74545.249758411 - 6337.338043467*log((double)_PR);
	return altitude;
}

void MS5611::setPressureOffset(int16_t _pressureOffset){
	pressureOffset = _pressureOffset;
}		