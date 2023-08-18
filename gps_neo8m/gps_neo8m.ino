#include <TinyGPS++.h>
#include <SoftwareSerial.h>
 
int RXPin = 10;
int TXPin = 9;
int GPSBaud = 9600;
int tempo = 0;

TinyGPSPlus gps;
 
SoftwareSerial gpsSerial(RXPin, TXPin);
 
void setup(){
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
}
 
void loop(){
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();
  if (millis()-tempo > 5000 && gps.charsProcessed() < 10){
    Serial.println("Sinal GPS não detectado");
    tempo = millis();
  }
}
 
void displayInfo(){
  if (gps.location.isValid()){
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
  }
  else
  Serial.println("Localização não detectada");
  Serial.print("Data: ");
  if (gps.date.isValid()){
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.month());//LEITURA DO MêS
    Serial.print("/");
    Serial.println(gps.date.year());//LEITURA DO ANO
  }
  else
    Serial.println("Erro");
  Serial.print("Hora: ");
  if (gps.time.isValid()){
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.hour() - 3);
    Serial.print(":");
    if (gps.time.minute() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10)
      Serial.print(F("0"));
    Serial.println(gps.time.second());
  }
  else
    Serial.println("Horário atual não detectado");
  Serial.println();
  delay(1000);
}