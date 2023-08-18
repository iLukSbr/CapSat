void setup() {
  Serial.begin(9600);

}

void loop() {
  Serial.print(analogRead(A7));
  Serial.print("\t");
  Serial.print(analogRead(A3));
  Serial.print("\t");
  Serial.println(analogRead(A6));
  delay(1000);
}
