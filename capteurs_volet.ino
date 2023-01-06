#include "DHT.h"
DHT dht(18, DHT11);

void setup() {
   dht.begin();
   Serial.begin(9600);
}

void loop() {
   float h = dht.readHumidity();
   float t = dht.readTemperature();

   Serial.print("Humidity: ");
   Serial.println(h);
   delay(2000);
   Serial.print("Temperature: ");
   Serial.println(t);
}
