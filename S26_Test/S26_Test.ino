

#define relay 12
#define led 13
#define button 0

bool state = false;

void handle() {
  if (state) {
//    client.publish(MQTT_LIGHT_STATE_TOPIC, ON, true);
    digitalWrite(relay, HIGH);
    digitalWrite(led, LOW);
  } else {
//    client.publish(MQTT_LIGHT_STATE_TOPIC, OFF, true);
    digitalWrite(relay, LOW);
    digitalWrite(led, HIGH);
  }
}

void setup() {
  pinMode(button, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(led, OUTPUT);
  
  digitalWrite(led, LOW);

  Serial.begin(115200);
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
}

