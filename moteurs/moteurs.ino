#include "controle_moteurs.h"
#include <WiFi.h>
#include <PubSubClient.h>
const char* ssid = "nicolas";
const char* password = "00000000";
const char* mqtt_server = "mqtt.ci-ciad.utbm.fr";
#define mqtt_port 1883
#define MQTT_USER "" 
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_RECEIVER_LUM "IF3B/ProjetVoletGroupe2/luminosite"
#define MQTT_SERIAL_RECEIVER_TEMP "IF3B/ProjetVoletGroupe2/temperature"
#define MQTT_SERIAL_RECEIVER_MAX_LUM "IF3B/ProjetVoletGroupe2/max/luminosite"
#define MQTT_SERIAL_RECEIVER_MAX_TEMP "IF3B/ProjetVoletGroupe2/max/temperature"
#define MQTT_SERIAL_RECEIVER_ACTION "IF3B/ProjetVoletGroupe2/action" // 0 ferme et 1 ouvre
#define MQTT_SERIAL_RECEIVER_MODE "IF3B/ProjetVoletGroupe2/mode" // 0 = automatique et 1 = manuel
#define MQTT_SERIAL_RECEIVER_CAPTEUR_ASSIGN "IF3B/ProjetVoletGroupe2/capteur" // de 1-4 pales, 5 = 1-2, 6 = 2-3, 7 = 3-4
#define MQTT_SERIAL_RECEIVER_PIR "IF3B/ProjetVoletGroupe2/PIR" // de 1-4 pales, 5 = 1-2, 6 = 2-3, 7 = 3-4


WiFiClient wifiClient;

PubSubClient client(wifiClient);


int active_mode = 0; // 0 auto et 1 manu
int capteur_1 = 1; // 1 - 4 pales, 5 pales 1-2, 6 pales 2-3, 7 pales 3-4 
int max_temp = 27;
int max_lum = 5000; // A changer pour mettre à l'echelle
int min_lum = 100; // A changer pour mettre à l'echelle

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
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}


void callback(char* topic, byte *payload, unsigned int length) {
    if(topic == MQTT_SERIAL_RECEIVER_MODE){
      Serial.println("Mode :");
      if(!*payload){ // 0 auto et 1 manuel
        active_mode = 0;
      } else {     
        active_mode = 1;
      }
    }
    if(topic == MQTT_SERIAL_RECEIVER_ACTION){
      if(active_mode == 0){ 
        if(!*payload){ // 0 ferme et 1 ouvre
          
          //int etat_final[4] = {0, 0, 0, 0};
          //orienter_volet(etat_final);
        } else {
          
          //int etat_final[4] = {1050, 1050, 1050, 1050};
          //orienter_volet(etat_final);
        }
      }
    }
    if(topic == MQTT_SERIAL_RECEIVER_CAPTEUR_ASSIGN){
      int capteur = *payload;
    }
    if(topic == MQTT_SERIAL_RECEIVER_PIR){
      // Gérer le chrono
    }
    if(topic == MQTT_SERIAL_RECEIVER_TEMP){
      if(*payload > max_temp){
        int etat_final[4] = {1050, 1050, 1050, 1050};
        orienter_volet(etat_final);
      }
    }
    if(topic == MQTT_SERIAL_RECEIVER_LUM){
      if(*payload < min_lum){
        int etat_final[4] = {1050, 1050, 1050, 1050};
        orienter_volet(etat_final);
      }
      if(*payload < max_lum){
        //Fermer la pale assigner (ici la pale est def par la variable : capteur_1
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
      client.subscribe(MQTT_SERIAL_RECEIVER_CAPTEUR_ASSIGN);
      client.subscribe(MQTT_SERIAL_RECEIVER_PIR);
      client.subscribe(MQTT_SERIAL_RECEIVER_LUM);
      client.subscribe(MQTT_SERIAL_RECEIVER_TEMP);
      
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishSerialData(char *serialData, char* type){
  if (!client.connected()) {
    reconnect();
  }
}

void loop() {
   client.loop();
 }
