#include <Wire.h>
#include <BH1750.h>
#define PIR 34
#include "DHT.h"
DHT dht(18, DHT11);

BH1750 lightMeter;

void setup(){
  Serial.begin(9600);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);
  // For Wemos / Lolin D1 Mini Pro and the Ambient Light shield use Wire.begin(D2, D1);

  lightMeter.begin();
 

  Serial.println(F("BH1750 Test begin"));

  pinMode(PIR, INPUT);

  dht.begin();
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");            //programme lumiere
  delay(500);
  int pirVal = digitalRead(PIR);

   if (pirVal == HIGH) {
      Serial.println("on");
      delay(500);
   }
                                    //programme sensor
   else {
      Serial.println("off");
      delay(500);
   }

   float h = dht.readHumidity();
   float t = dht.readTemperature();

   Serial.print("Humidity: ");     //programme temperature
   Serial.println(h);
   delay(500);
   Serial.print("Temperature: ");
   Serial.println(t);
   delay(500);
}
