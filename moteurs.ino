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
#define MQTT_SERIAL_RECEIVER_TIME_OPEN "IF3B/ProjetVoletGroupe2/time/open"
#define MQTT_SERIAL_RECEIVER_TIME_CLOSE "IF3B/ProjetVoletGroupe2/time/close" // 
#define MQTT_SERIAL_RECEIVER_CAPTEUR_1 "IF3B/ProjetVoletGroupe2/capteur/1" // de 1 à 4 pour l'assignation des pales
#define MQTT_SERIAL_RECEIVER_CAPTEUR_2 "IF3B/ProjetVoletGroupe2/capteur/2"// de 1 à 4
#define MQTT_SERIAL_RECEIVER_MODE "IF3B/ProjetVoletGroupe2/mode" // 0 = automatique et 1 = manuel
#define MQTT_SERIAL_RECEIVER_ACTION "IF3B/ProjetVoletGroupe2/action" // 0 ferme et 1 ouvre


WiFiClient wifiClient;

PubSubClient client(wifiClient);

int i = 0;
bool test = true;

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
      client.subscribe(MQTT_SERIAL_RECEIVER_TIME_OPEN);
      client.subscribe(MQTT_SERIAL_RECEIVER_TIME_CLOSE);
      client.subscribe(MQTT_SERIAL_RECEIVER_CAPTEUR_1);
      client.subscribe(MQTT_SERIAL_RECEIVER_CAPTEUR_2);
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
      Serial.println("Action :");
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
      if(
      if(!*payload){ // 0 ferme et 1 ouvre
        Serial.println("fermeture");
        //Fermeture volet
      } else {
        Serial.println("close function");
        //Ouverture volet
      }
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
  } else {
    client.publish(MQTT_SERIAL_PUBLISH_LUM, serialData);
  }
}

void loop() {
   client.loop();
   
   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     char* type = "temp";
     publishSerialData(mun, type);
   }
 }
  
  
