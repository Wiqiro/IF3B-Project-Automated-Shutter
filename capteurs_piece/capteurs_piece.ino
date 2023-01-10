#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>

#define PIR 27

Adafruit_SSD1306 display(128, 64, &Wire, -1);
BH1750 lightMeter;
DHT dht(18, DHT11);

void ecran_hello_world();

void setup() {
  Serial.begin(115200);

  Wire.begin(); 
  lightMeter.begin(); //setup capteur luminosité
  pinMode(PIR, INPUT); //setup capteur pir
  dht.begin(); //setup capteur température
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //setup ecran

  ecran_hello_world();
}

void loop() {
  float lux = lightMeter.readLightLevel();
  bool pir= digitalRead(PIR);
  float humidite = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.printf("Light: %.2flx\tPIR: %d\tTemperature: %.2fC\n", lux, pir, temperature);
    delay(500);
}




void ecran_hello_world() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hello, world!");
  display.display(); 
}
