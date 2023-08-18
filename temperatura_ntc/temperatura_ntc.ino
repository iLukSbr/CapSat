#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <SmoothThermistor.h>

#define SENSOR_PIN             A0
#define REFERENCE_RESISTANCE   9870
#define NOMINAL_RESISTANCE     10000
#define NOMINAL_TEMPERATURE    25
#define B_VALUE                3950
#define SMOOTHING_FACTOR 5

static Thermistor* thermistor = nullptr;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Thermistor* originThermistor = new NTC_Thermistor(SENSOR_PIN,REFERENCE_RESISTANCE,NOMINAL_RESISTANCE,NOMINAL_TEMPERATURE,B_VALUE);
  thermistor = new SmoothThermistor(originThermistor, SMOOTHING_FACTOR);
}

void loop() {
  const double celsius = thermistor->readCelsius();
  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.println(" C, ");
  delay(500);
}
