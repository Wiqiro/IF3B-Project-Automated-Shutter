#include <WiFi.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.
const char* ssid = "nicolas";
const char* password = "123456789";
const char* mqtt_server = "mqtt.ci-ciad.utbm.fr";
#define mqtt_port 1883
#define MQTT_USER "" 
#define MQTT_PASSWORD ""
//#define MQTT_SERIAL_PUBLISH_LUM "IF3B/ProjetVoletGroupe2"
//#define MQTT_SERIAL_RECEIVER_CH "IF3B/ProjetVoletGroupe2"
#define MQTT_SERIAL_PUBLISH_LUM "IF3B/ProjetVoletGroupe2/luminosite"
#define MQTT_SERIAL_PUBLISH_TEMP "IF3B/ProjetVoletGroupe2/temperature"
#define MQTT_SERIAL_RECEIVER_TEMPS_OUVERTURE "IF3B/ProjetVoletGroupe2/temps/ouverture"
#define MQTT_SERIAL_RECEIVER_TEMPS_FERMETURE "IF3B/ProjetVoletGroupe2/temps/fermeture"
#define MQTT_SERIAL_RECEIVER_CAPTEUR_1 "IF3B/ProjetVoletGroupe2/capteur/1" // de 1 à 4 pour l'assignation des pales
#define MQTT_SERIAL_RECEIVER_MODE "IF3B/ProjetVoletGroupe2/mode" // 0 = automatique et 1 = manuel
#define MQTT_SERIAL_RECEIVER_ACTION "IF3B/ProjetVoletGroupe2/action" // 0 ferme et 1 ouvre


WiFiClient wifiClient;

PubSubClient client(wifiClient);

int i = 0;
bool test = true;

int active_mode = 0; // 0 auto et 1 manu
int capteur_1 = 1; // 1 - 4 pales, 5 pales 1-2, 6 pales 2-3, 7 pales 3-4 
int temps_ouverture = 6;
int temps_fermeture = 18;

void volet_close(){
    Serial.println("Le volet se ferme");
  
}
void volet_open(){
    Serial.println("Le volet s'ouvre");
}

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
      client.subscribe(MQTT_SERIAL_RECEIVER_TEMPS_OUVERTURE);
      client.subscribe(MQTT_SERIAL_RECEIVER_TEMPS_FERMETURE);
      client.subscribe(MQTT_SERIAL_RECEIVER_CAPTEUR_1);
      client.subscribe(MQTT_SERIAL_RECEIVER_MODE);
      
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



    //Serial.println(topic); 
    //Serial.write(payload, length);
    if(topic == MQTT_SERIAL_RECEIVER_MODE){
      Serial.println("Mode :");
      if(!*payload){ // 0 manuel et 1 auto
        Serial.println("fermeture");
        //Fermeture volet
      } else {
        Serial.println("close function");
        //Ouverture volet
      }
    }
    if(topic == MQTT_SERIAL_RECEIVER_ACTION){
      Serial.println("Action :");
      if(active_mode == 0){ 
        if(!*payload){ // 0 ferme et 1 ouvre
          Serial.println("fermeture");
          int etat_final[4] = {0, 0, 0, 0};
          orienter_volet(etat_final);
        } else {
          Serial.println("ouverture");
          int etat_final[4] = {1050, 1050, 1050, 1050};
          orienter_volet(etat_final);
        }
      }
    }
    if(topic == MQTT_SERIAL_RECEIVER_TEMPS_OUVERTURE){
      temps_ouverture = *payload;
      Serial.write(temps_ouverture);
    }
    if(topic == MQTT_SERIAL_RECEIVER_TEMPS_FERMETURE){
      temps_fermeture = *payload;
      Serial.write(temps_fermeture);
    }
}


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  reconnect();
  
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
  if(type = "mode"){
    client.publish(MQTT_SERIAL_RECEIVER_MODE, serialData);
  }
}

void loop() {
   client.loop();
   publishSerialData(mun, type); // Fonction pour print 
   
   if( active_mode == 0){// On vérifie si le mode est manuel ou auto
     publishSerialData("Automatique", "mode");
   } else {
     publishSerialData("Manuel", "mode");
   }
   
   
 }
  
  
