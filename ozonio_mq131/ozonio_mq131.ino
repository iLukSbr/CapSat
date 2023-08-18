#include <MQUnifiedsensor.h>

#define placa "Arduino MEGA"
#define Voltage_Resolution 5
#define pin A0 //Analog input 0 of your arduino
#define type "MQ-131" //MQ131
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO
#define RatioMQ131CleanAir 15 //RS / R0 = 15 ppm

MQUnifiedsensor MQ131(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup(){
  Serial.begin(9600); //Init serial port
  MQ131.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ131.setA(23.943); MQ131.setB(-1.11); // Configure the equation to to calculate O3 concentration
  MQ131.init();
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ131.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ131.calibrate(RatioMQ131CleanAir);
    Serial.print(".");
  }
  MQ131.setR0(calcR0/10);
  Serial.println("  done!.");
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  MQ131.serialDebug(true);
  MQ131.setOffset(0.1);
}

void loop(){
  MQ131.update(); // Update data, the arduino will read the voltage from the analog pin
  MQ131.readSensorR0Rs(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  Serial.print(MQ131.getConcentration(),4);
  Serial.println(" ppb");
  delay(500); //Sampling frequency
}