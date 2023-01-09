#include <Wire.h>
#include <BH1750.h>

#include "DHT.h"

#define PIR 14
DHT dht(18, DHT11);


BH1750 lightMeter;

void setup() {
  Serial.begin(115200);

  Wire.begin(); //setup capteur luminosité
  lightMeter.begin();

  pinMode(PIR, INPUT); //setup capteur pir

  dht.begin(); //setup capteur température
}

void loop() {
  float lux = lightMeter.readLightLevel();
  bool pir= digitalRead(PIR);
  float humidite = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.printf("Light: %.2flx\tPIR: %d\tTemperature: %.2fC\n", lux, pir, temperature);
    delay(500);
}
