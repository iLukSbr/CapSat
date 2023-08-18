int sensorPin = A9; //define analog pin 2
double luminosidade = 0.0; 
void setup() {
	Serial.begin(9600); 
} 
void loop() {
	luminosidade = (double)map(analogRead(sensorPin),0,1023,0,100000000000)/1000000000; 
	Serial.println(luminosidade); // light intensity
								// high values for bright environment
								// low values for dark environment
	delay(100); 
}