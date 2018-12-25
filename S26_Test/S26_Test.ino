#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "passwords.h"

#define relay 12
#define led 13
#define button 0

bool state = false;

long previousMillis = 0;
int ledState = LOW;    

long interval = 5000;



#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const char* WIFI_SSID = wifiUser;
const char* WIFI_PASSWORD = wifiPass;

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "T1-1";
const PROGMEM char* MQTT_SERVER_IP = "192.168.1.12";                   // server ip (eg. 192.168.1.10)
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;                        // MQTT Port (Deafult: 1883)
const PROGMEM char* MQTT_USER = "";                              // MQTT Username
const PROGMEM char* MQTT_PASSWORD = "";                          // MQTT Password

// MQTT: topics
const char* MQTT_LIGHT_STATE_TOPIC = "asdfbajsdfbjksad/status";      // topic to publish state changes / double press actions
const char* MQTT_LIGHT_COMMAND_TOPIC = "asdfbajsdfbjksad/switch";    // topic to recieve commands

// payloads by default
const char* ON1 = "ON";
const char* OFF1 = "OFF";

WiFiClient wifiClient;
PubSubClient client(wifiClient);


void handle() {
  if (state) {
    client.publish(MQTT_LIGHT_STATE_TOPIC, ON, true);
    digitalWrite(relay, HIGH);
    digitalWrite(led, LOW);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC, OFF, true);
    digitalWrite(relay, LOW);
    digitalWrite(led, HIGH);
  }
}
// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  
  // handle message topic
  if (String(MQTT_LIGHT_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    
    if (payload.equals(String(ON))) {
        state = true;
        handle();
    } else if (payload.equals(String(OFF))) {
        state = false;
        handle();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
      // Once connected, publish an announcement...
      handle();
      // ... and resubscribe
      client.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
    }
}




void setup() {
  pinMode(button, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(led, OUTPUT);
  
  digitalWrite(led, LOW);

  Serial.begin(115200);

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  
  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  if (digitalRead(0) == 0)
  {
    if (state)
    {
      state = false;
    }
    else
    {
      state = true;
    }
    handle(); 
  }

  
  unsigned long currentMillis = millis();
  // Retry wifi connection every 5 seconds if failed
  if (WiFi.status() == WL_CONNECTED)
  {
    if(currentMillis - previousMillis > interval) {
     if (!client.connected()) {
        reconnect();
      }
      previousMillis = currentMillis;
    }
    // Only run MQTT control when connected to wifi
    client.loop();
  }
}

