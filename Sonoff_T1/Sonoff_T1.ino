#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "passwords.h"

// Made By Dhiluka
// 2018-12-22

// Double tap to toggle whole device
bool toggleAll = true;

// pin numbers for pads
#define pad1 0
#define pad2 9
#define pad3 10

//pin numbers for relays
#define relay1 12
#define relay2 5
#define relay3 4

// pin number for LED under Wifi icon
#define notif 13

// time in Millis for long press
#define longPress 300



// booleans to store switch positions
bool s1 = false;
bool s2 = false;
bool s3 = false;



// reconnect without holding the program
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
const PROGMEM char* MQTT_USER = MQTTUser;                              // MQTT Username
const PROGMEM char* MQTT_PASSWORD = MQTTPass;                          // MQTT Password

// MQTT: topics
const char* MQTT_LIGHT_STATE_TOPIC = "home/sonoff/light1/status";      // topic to publish state changes / double press actions
const char* MQTT_LIGHT_COMMAND_TOPIC = "home/sonoff/light1/switch";    // topic to recieve commands

// payloads by default
const char* ON1 = "ON1";
const char* OFF1 = "OFF1";

const char* ON2 = "ON2";
const char* OFF2 = "OFF2";

const char* ON3 = "ON3";
const char* OFF3 = "OFF3";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called to handle light state output and MQTT state publishing
void handle1() {
  if (s1) {
    client.publish(MQTT_LIGHT_STATE_TOPIC, ON1, true);
    digitalWrite(relay1, HIGH);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC, OFF1, true);
    digitalWrite(relay1, LOW);
  }
}

void handle2() {
  if (s2) {
    client.publish(MQTT_LIGHT_STATE_TOPIC, ON2, true);
    digitalWrite(relay2, HIGH);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC, OFF2, true);
    digitalWrite(relay2, LOW);
  }
}

void handle3() {
  if (s3) {
    client.publish(MQTT_LIGHT_STATE_TOPIC, ON3, true);
    digitalWrite(relay3, HIGH);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC, OFF3, true);
    digitalWrite(relay3, LOW);
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
    
    if (payload.equals(String(ON1))) {
        s1 = true;
        handle1();
    } else if (payload.equals(String(OFF1))) {
        s1 = false;
        handle1();
    }

    if (payload.equals(String(ON2))) {
        s2 = true;
        handle2();
    } else if (payload.equals(String(OFF2))) {
        s2 = false;
        handle2();
    }

    if (payload.equals(String(ON3))) {
        s3 = true;
        handle3();
    } else if (payload.equals(String(OFF3))) {
        s3 = false;
        handle3();
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
      handle1();
      handle2();
      handle3();
      // ... and resubscribe
      client.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
    }
}

void setup() {
  // Set up pads
  pinMode(pad1, INPUT);
  pinMode(pad2, INPUT);
  pinMode(pad3, INPUT);

  // Set up relays
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  // Set up LED
  pinMode(notif, OUTPUT);

  // Turn off LED
  digitalWrite(notif, HIGH);

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
  digitalWrite(notif, LOW);
}

void loop() {
  
// Buttons
  if (digitalRead(pad1) == 0)
  {
    delay(10);
    if (digitalRead(pad1) == 0)
    {
      delay(longPress);                //long press
      if (digitalRead(pad1) == 0)
      {
        s1 = true;
        s2 = true;
        s3 = true;

        handle1();
        handle2();
        handle3();
        client.publish(MQTT_LIGHT_STATE_TOPIC, "long1", true);
      }
      else
      {
        if (s1)
        {
          s1 = false;
        }
        else
        {
          s1 = true;
        }
        handle1();
      }
    }
    while (digitalRead(pad1) == 0);
  }

  if (digitalRead(pad2) == 0)
  {
    delay(10);
    if (digitalRead(pad2) == 0)
    {
      delay(longPress);                //long press
      if (digitalRead(pad2) == 0)
      {
        client.publish(MQTT_LIGHT_STATE_TOPIC, "long2", true);
      }
      else
      {
        if (s2)
        {
          s2 = false;
        }
        else
        {
          s2 = true;
        }
        handle2();
      }
    }
    while (digitalRead(pad2) == LOW);
  }

  if (digitalRead(pad3) == 0)
  {
    delay(10);
    if (digitalRead(pad3) == 0)
    {
      delay(longPress);                //long press
      if (digitalRead(pad3) == 0)
      {
        s1 = false;
        s2 = false;
        s3 = false;
  
        handle1();
        handle2();
        handle3();
        client.publish(MQTT_LIGHT_STATE_TOPIC, "long3", true);

      }
      else
      {
        if (s3)
        {
          s3 = false;
        }
        else
        {
          s3 = true;
        }
        handle3();
      }
    }
    while (digitalRead(pad3) == LOW);
  }

  // LED on when connected to Wifi
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(notif, LOW);
  }
  else
  {
    digitalWrite(notif, HIGH);
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
