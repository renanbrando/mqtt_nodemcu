#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

// Replace with your network credentials and mqtt settings
const char* ssid = "Keysurf";
const char* password = "(Keysurf)#";
const char* mdnsName = "nodemcu";
const char* mqttServer = "iot.eclipse.org";
const char* mqttUser = "";
const char* mqttPassword = "";
const char* mqttPubTopic = "nodemcu";
const char* mqttSubTopic = "test";

int led = 5;
int button = 16; // push button is connected
int temp = 0;    // temporary variable for reading the button pin status



WiFiClient espClient;
PubSubClient client(espClient);
MDNSResponder mdns;

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  WiFi.begin(ssid, password);
  Serial.begin(115200); 
  Serial.println("Setting up");
  delay(5000);
  // Wait for connection and flash led till it connects
  // when connected; turn the led off
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);
    delay(200);
    Serial.print(".");
  }
  
  Serial.print("Connected to ");  Serial.println(ssid);
  Serial.print("IP address: ");   Serial.println(WiFi.localIP());

  // lets mDNS on the received ip
  if (mdns.begin(mdnsName, WiFi.localIP())) {
    //Serial.println("mDNS started for " + mdnsName + ".local");
  }

  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived ["); Serial.print(topic); Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if 1 was received as first character
  if ((char)payload[0] == '1') {
    Serial.println("Turning LED on");
    digitalWrite(led, HIGH);
  }
  else if ((char)payload[0] == '0') {
    Serial.println("Turning LED off");
    digitalWrite(led, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("connected");
      client.publish(mqttPubTopic, "Button Pressed!");  //publishing its status
      client.subscribe(mqttSubTopic);             //subscribing to topic
    }
    else {
      Serial.print("Failed, state="); Serial.print(client.state());
      Serial.println(" will try again in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  temp = digitalRead(button);
  Serial.println(temp);
     
     if (temp == HIGH) {
        digitalWrite(led, HIGH);
        Serial.println("LED Turned ON");
        delay(1000);
        if (!client.connected())
          reconnect();
       }
     else {
        digitalWrite(led, LOW);
        Serial.println("LED Turned OFF");
        delay(1000);
       }
       
  client.loop();
}
