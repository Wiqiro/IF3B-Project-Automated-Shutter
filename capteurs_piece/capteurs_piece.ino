#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "nicolas";
const char* password = "00000000";
const char* mqtt_server = "mqtt.ci-ciad.utbm.fr";
#define mqtt_port 1883
#define MQTT_USER "" 
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_LUM "IF3B/ProjetVoletGroupe2/luminosite"
#define MQTT_SERIAL_PUBLISH_TEMP "IF3B/ProjetVoletGroupe2/temperature"
#define MQTT_SERIAL_PUBLISH_PIR "IF3B/ProjetVoletGroupe2/pir"
#define MQTT_SERIAL_RECEIVER_MODE "IF3B/ProjetVoletGroupe2/mode" // 0 = automatique et 1 = manuel

#define PIR 27

WiFiClient wifiClient;

PubSubClient client(wifiClient);

int i = 0;
bool test = true;

int active_mode = 0; // 0 auto et 1 manu

Adafruit_SSD1306 display(128, 64, &Wire, -1);
BH1750 lightMeter;
DHT dht(18, DHT11);



void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("waiting pairing...");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}



void ecran_hello_world();

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  reconnect();
  
  //Wire.begin(); 
  //lightMeter.begin(); //setup capteur luminosité
  //pinMode(PIR, INPUT); //setup capteur pir
  //dht.begin(); //setup capteur température
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //setup ecran

  //ecran_hello_world();
}







void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("IF3B/test/serialdata/tx", "reconnected");
      // ... and resubscribe
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    if(topic == MQTT_SERIAL_RECEIVER_MODE){
        active_mode = *payload;
    }
}



void publishSerialData(char *serialData, char* type){
  if (!client.connected()) {
    reconnect();
  }
  if(type = "lum"){
    client.publish(MQTT_SERIAL_PUBLISH_LUM, serialData);
  }
  if(type = "temp"){
    client.publish(MQTT_SERIAL_PUBLISH_TEMP, serialData);
  }
  if(type = "pir"){
    client.publish(MQTT_SERIAL_PUBLISH_PIR, serialData);
  }
}





void loop() {
  char * lux_buffer;
  char * pir_buffer;
  char * temp_buffer;

  
  float lux = lightMeter.readLightLevel();
  sprintf(lux_buffer, "%f", lux);
  publishSerialData(lux_buffer, "lum");
  bool pir= digitalRead(PIR);
  if(pir){
    pir_buffer = "1";
  } else {
    pir_buffer = "0";
  }
  publishSerialData(pir_buffer, "pir"); 
  float temp = dht.readTemperature();
  sprintf(temp_buffer, "%f", temp);
  publishSerialData(temp_buffer, "temp");

  delay(500);
  client.loop();
 
  
}





void ecran_hello_world() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Hello, world!");
  display.display(); 
}


  
