//#pragma once

#include <string>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// MQTT settings
const String ID = "2";
const String BROKER = "192.168.1.102";
const String CLIENT_NAME = "pico_"+ID;
const String TOPIC = "80";

String in_txt;
bool callback = false;

WiFiClient espClient;
PubSubClient client(espClient); // Setup MQTT client

// ---- MQTT


// Handle incomming messages from the broker
void clientCallback(char* topic, byte* payload, unsigned int length) {
  //String response;

  for (int i = 0; i < length; i++) {
    in_txt += (char)payload[i];
    //response += (char)payload[i];
  }
  // Serial.print("Message arrived [");
  // Serial.print(TOPIC.c_str());
  // Serial.print("] ");
  // Serial.println(response);
  //in_txt = response;
  callback = true;

  /*

  // Obtén la longitud de la cadena original
  size_t len = response.length();

  // Reserva memoria para una cadena de caracteres (char[]) con el tamaño adecuado
  char *response_char = (char *)malloc(len + 1);  // +1 para el carácter nulo '\0'

  // Copia la cadena original en la cadena convertida
  response.toCharArray(response_char, len + 1);

  //return response;
  */
}

void reconnectMQTTClient() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(CLIENT_NAME.c_str())) {
      Serial.print("connected to Broker: ");
      Serial.println(BROKER.c_str());
      // Topic(s) subscription
      client.subscribe(TOPIC.c_str());
      Serial.print("Subscribed to Topic: ");
      Serial.println(TOPIC.c_str());
      client.setCallback(clientCallback); 
    }
    else {
      Serial.print("Retying in 5 seconds - failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void createMQTTClient() {
  client.setServer(BROKER.c_str(), 1883);
  //client.setCallback(clientCallback);
  reconnectMQTTClient();
}


void disconectWiFiMQTT(){
  client.disconnect();
  WiFi.disconnect();
}
