#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>


#include "controle_moteurs.h"

const char* ssid = "findx3";
const char* password = "12341234";
const char* mqtt_server = "mqtt.ci-ciad.utbm.fr";
#define mqtt_port 1883
#define MQTT_USER "" 
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_LUM_EXT "IF3B/ProjetVoletGroupe2/luminosite_ext"
#define MQTT_SERIAL_RECEIVER_ACTION "IF3B/ProjetVoletGroupe2/action" // 0 ferme et 1 ouvre
#define MQTT_SERIAL_RECEIVER_MODE "IF3B/ProjetVoletGroupe2/mode" // 0 = automatique et 1 = manuel
#define MQTT_SERIAL_RECEIVER_PALES "IF3B/ProjetVoletGroupe2/pales"
#define MQTT_SERIAL_RECEIVER_ETAT_LUM "IF3B/ProjetVoletGroupe2/etat_lum"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

#define BOUTON1 23
#define BOUTON2 35
BH1750 lightMeter;


bool mode_volet = 0; // 0 auto et 1 manu
int assignation_1 = 1; // 1 - 4 pales, 5 pales 1-2, 6 pales 2-3, 7 pales 3-4 
int max_temp = 27;
int max_lum = 5000; //fermeture de la pale si supérieur
int min_lum = 100; // allumage de la diode si inférieur

int pir_temps = 0;
float temperature = 0;
float lux_piece = 0;


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



void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500); // Set time out for 

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();

  Wire.begin(); 
  lightMeter.begin(); //setup capteur luminosité

  pinMode(BOUTON1, INPUT);
  pinMode(BOUTON2, INPUT);
  calibrer_volet(BOUTON1); 
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
  } else if (strcmp(topic,MQTT_SERIAL_RECEIVER_ACTION) == 0) {
      if (payload[0] == '0') { // 0 ferme et 1 ouvre
        Serial.println("Fermeture du volet");
        orienter_volet(POSITION_FERMEE);
      } else {
        Serial.println("Ouverture du volet");
        orienter_volet(POSITION_OUVERTE);
      }
  } else if (strcmp(topic, MQTT_SERIAL_RECEIVER_PALES) == 0) {
    assignation_pales[0] = payload[0] - '1';
    assignation_pales[1] = payload[1] - '1';
    assignation_pales[2] = payload[2] - '1';
    assignation_pales[3] = payload[3] - '1';
  } else if (strcmp(topic, MQTT_SERIAL_RECEIVER_ETAT_LUM) == 0)  {
    for (int pale = 0; pale < 4; pale++) {
      if (assignation_pales[pale]) {
        if (payload[0] == '0' && etat_pale[pale] < 550) {
          bouger_pale(pale, 50);
        } else if (payload[0] == '1' && etat_pale[pale] > 0) {
          bouger_pale(pale, -50);
        }
      }
    }
  } 
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
      client.subscribe(MQTT_SERIAL_RECEIVER_ACTION);
      client.subscribe(MQTT_SERIAL_RECEIVER_MODE);
      client.subscribe(MQTT_SERIAL_RECEIVER_PALES);
      client.subscribe(MQTT_SERIAL_RECEIVER_ETAT_LUM);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishSerialData(char *serialData, char type){
  /*if (!client.connected()) {
    reconnect();
  }*/
  if (type == 'l') {
    client.publish(MQTT_SERIAL_PUBLISH_LUM_EXT, serialData);
  } else if (type == 'm') {
    client.publish(MQTT_SERIAL_RECEIVER_MODE, serialData);
  }
}

time_t timer = time(NULL);

void loop() {
  if (time(NULL) - timer > 1) {
    timer = time(NULL);
    float lux_val = lightMeter.readLightLevel();
    char lux_buffer[6];
    sprintf(lux_buffer, "%d", (int)lux_val);
    publishSerialData(lux_buffer, 'l');
  }
  

  if (digitalRead(BOUTON2)) {
    mode_volet = !mode_volet;
    if (mode_volet) {
      publishSerialData("1", 'm');
    } else {
      publishSerialData("0", 'm');
    }
    delay(500);
  }
  if (digitalRead(BOUTON1)) {
    Serial.println("Bouton 1 presse");
    if (mode_volet == 0) {
    Serial.println("Calibrage");
      delay(500);
      calibrer_volet(BOUTON1);
    } else {
      Serial.println("Fermeture/ouverture");
      if (etat_pale[0] == 0 && etat_pale[1] == 0 && etat_pale[2] == 0 && etat_pale[3] == 0) {
        orienter_volet(POSITION_OUVERTE);
      } else {
        orienter_volet(POSITION_FERMEE);
      }
    }
  }

  client.loop();
  
}

