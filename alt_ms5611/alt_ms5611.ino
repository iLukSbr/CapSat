#include <MS5611.h>
#include <math.h>

MS5611 baro;
double pressao;
double temperatura;
double altitude;

void setup() {
  baro = MS5611();
  baro.begin();
  baro.setPressureOffset_Pa(-1890.0);
  Serial.begin(9600);
  delay(2);
}

void loop() {
  pressao = baro.getPressure_Pa();
  temperatura = baro.getTemperature_C();
  altitude = baro.getAltitude_m();
  Serial.print(pressao);
  Serial.print(" Pa\t");
  Serial.print(temperatura);
  Serial.print(" °C\t");
  Serial.print(altitude);
  Serial.println(" m");
  delay(1000);
}