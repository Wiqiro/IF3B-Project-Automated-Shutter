#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "findx3";
const char* password = "12341234";
const char* mqtt_server = "mqtt.ci-ciad.utbm.fr";
#define mqtt_port 1883
#define MQTT_USER "" 
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_LUM "IF3B/ProjetVoletGroupe2/luminosite_int"
#define MQTT_SERIAL_PUBLISH_TEMP "IF3B/ProjetVoletGroupe2/temperature"
#define MQTT_SERIAL_PUBLISH_PIR "IF3B/ProjetVoletGroupe2/pir"
#define MQTT_SERIAL_RECEIVER_MODE "IF3B/ProjetVoletGroupe2/mode" // 0 = automatique et 1 = manuel
#define MQTT_SERIAL_RECEIVER_LED "IF3B/ProjetVoletGroupe2/led" // 0 = automatique et 1 = manuel


#define PIR 27
#define LED 26

WiFiClient wifiClient;

PubSubClient client(wifiClient);

int i = 0;
bool test = true;

bool mode_volet = 0; // 0 auto et 1 manu

Adafruit_SSD1306 display(128, 64, &Wire, -1);
BH1750 lightMeter;
DHT dht(5, DHT11);



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
  
  Wire.begin(); 
  lightMeter.begin(); //setup capteur luminosité
  pinMode(PIR, INPUT); //setup capteur pir
  pinMode(LED, OUTPUT);
  dht.begin(); //setup capteur température
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //setup ecran

  ecran_hello_world();
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
      client.subscribe(MQTT_SERIAL_RECEIVER_MODE);
      client.subscribe(MQTT_SERIAL_RECEIVER_LED);
      
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
  if (strcmp(topic,MQTT_SERIAL_RECEIVER_MODE) == 0) {
    if (payload[0] == '0') { // 0 auto et 1 manuel : traduire en chiffre
      mode_volet = 0;
      Serial.println("Mode automatique");
    } else {     
      mode_volet = 1;
      Serial.println("Mode manuel");
    }
  } else if (strcmp(topic, MQTT_SERIAL_RECEIVER_LED) == 0) {
    if (payload[0] == '0') { // 0 auto et 1 manuel : traduire en chiffre
      digitalWrite(LED, LOW);
      Serial.println("LED eteinte");
    } else {     
      digitalWrite(LED, HIGH);
      Serial.println("LED allumee");
    }
  }
}



void publishSerialData(char serialData[], char type){
  //if (!client.connected()) {
  //  reconnect();
  //}
  if(type == 'l'){
    client.publish(MQTT_SERIAL_PUBLISH_LUM, serialData);
  }
  if(type == 't'){
    client.publish(MQTT_SERIAL_PUBLISH_TEMP, serialData);
  }
  if(type == 'p'){
    client.publish(MQTT_SERIAL_PUBLISH_PIR, serialData);
  }
}





void loop() { 
  client.loop();

  bool pir_val = digitalRead(PIR);
  float lux_val = lightMeter.readLightLevel();
  float dht_val = dht.readTemperature();


  char lux_buffer[6];
  char pir_buffer[2];
  char dht_buffer[3];


  
  if (pir_val) {
    strcpy(pir_buffer, "1");
  } else {
    strcpy(pir_buffer, "0");
  }
  sprintf(dht_buffer, "%d", (int)dht_val);
  sprintf(lux_buffer, "%d", (int)lux_val);

  publishSerialData(dht_buffer, 't');
  publishSerialData(lux_buffer, 'l');
  publishSerialData(pir_buffer, 'p');
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.printf("Volet IF3B\n");
  display.setTextSize(1);
  if (mode_volet) {
    display.println("Mode: manuel");
  } else {
    display.println("Mode: auto");
  }
  //display.printf("Mode: %d", mode_volet);
  display.printf("\nTemperature: %.1f\n\nLuminosite int: %.1f\n", dht_val, lux_val, lux_val);
  display.display(); 
  delay(500);
}





void ecran_hello_world() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Hello, world!");
  display.display(); 
}


  
