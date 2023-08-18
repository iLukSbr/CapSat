#include <ArduinoJson.h>// CÓDIGO TESTE
#include <HTTPClient.h>
#include <WiFi.h>
 
const char *ssid = "OBSAT";
const char *password = "OBSAT2023";
 
void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado.");
  postDataToServer();
}
 
void loop(){
}
 
void postDataToServer(){
  HTTPClient http;
  http.begin("https://obsat.org.br/teste_post/envio.php");
  http.addHeader("Content-Type", "application/json");
  Serial.println("Iniciando JSON.");
  StaticJsonDocument<1000> doc;
  doc["equipe"] = 77;
  doc["bateria"] = 98.5;
  doc["temperatura"] = 15.4;
  doc["pressao"] = 101300;
  doc["giroscopio"][0] = 42;
  doc["giroscopio"][1] = 90;
  doc["giroscopio"][2] = 30;
  doc["acelerometro"][0] = 10;
  doc["acelerometro"][1] = 3;
  doc["acelerometro"][2] = 4;
  JsonObject payload = doc.createNestedObject("payload");
  payload["gas"] = 3.1415;
  JsonObject classe = payload.createNestedObject("classe");
  classe["mag"][0] = 12;
  classe["mag"][1] = 123;
  classe["mag"][2] = 543;
  classe["chuva"] = "seco";
  String requestBody;
  Serial.println("Dados completos.");
  serializeJson(doc, Serial);
  serializeJson(doc, requestBody);
  int httpResponseCode = http.POST(requestBody);
  if(httpResponseCode>0){
    String response = http.getString();
    Serial.println();
    Serial.println(httpResponseCode);
    Serial.println(response);
  }
  else{
    Serial.print("Error occurred while sending HTTP POST: ");
    Serial.println(http.errorToString(httpResponseCode));
  }
}