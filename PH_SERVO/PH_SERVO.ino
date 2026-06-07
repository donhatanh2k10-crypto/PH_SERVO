#include <ESP32Servo.h>
#define sensorPin 34
#include <HTTPClient.h>
#include "WiFi.h"
String url_base = "https://api.thingspeak.com/update?api_key=1CQIACIFXAKBNJA3";
unsigned long avgValue;
Servo myServo;
void setup() {
  myServo.attach(18);  
  Serial.begin(115200);
   WiFi.begin("BKSTAR_T.302", "stemstar");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("delay 1000ms");
    delay(1000);
  }
  Serial.println("Wifi Connected!");
}

void loop() {
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);
    delay(3);
  }
  int mang[10];
  for (int i = 0; i < 10; i++) {
    mang[i] = analogRead(sensorPin);
    delay(1000);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (mang[i] > mang[j]) {
        int x = mang[i];
        mang[i] = mang[j];
        mang[j] = x;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++) {
    avgValue += mang[i];
  }
  float voltage = (float)avgValue * 3.3 / 4095.0 / 6.0;
  float phValue = 2.0 * voltage + 0.70;

  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V   pH: ");
  Serial.println(phValue, 2);
  HTTPClient http;
   String url_thingspeak = url_base + "&field1=" + float(voltage) + "&field2=" + float(phValue);
  http.begin(url_thingspeak);
   int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  

  for (int pos = 180; pos >= 0; pos--) {
    myServo.write(pos);
    delay(3);
  }

  delay(1000);

}