#include <WiFi.h>
#include <PubSubClient.h>
#include <vector>
#include <ArduinoJson.h>

typedef struct TrafficLight {
  bool red;
  bool yellow;
  bool green;
} TrafficLight;

std::vector<TrafficLight> modes = {
    {false, false, true},
    {false, false, false},
    {false, true, false},
    {true, false, false},
    {true, true, false},
};

int redPin = 12;
int yellowPin = 14;
int greenPin = 27;

const char* ssid = "MI 8";    // Your Wi-Fi Name
const char* password = "13579246";   // Wi-Fi Password

const char* mqttServer = "srv2.clusterfly.ru";
const int mqttPort = 9991;
const char* mqttUser = "user_1f40dd56";
const char* mqttPassword = "FYXS8AV-U-pgH";
const char* mqttTopic = "user_1f40dd56/temp";

WiFiClient espClient;
PubSubClient client(espClient);

void switch_light(int mode) {
    // Get traffic light states from the modes array
    auto[redState, yellowState, greenState] = modes[mode];
    
    // Control traffic light pins according to the obtained states
    digitalWrite(redPin, (int8_t)redState);
    digitalWrite(yellowPin, (int8_t)yellowState);
    digitalWrite(greenPin, (int8_t)greenState);
}

void setup_wifi() {
    delay(10);
    Serial.println("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;
    for (int i = 0; i < length; i++) {
        messageTemp += (char)message[i];
    }
    Serial.println(messageTemp);
    
    DynamicJsonDocument doc(200);
    DeserializationError error = deserializeJson(doc, messageTemp);
    if (error) 
        return;
    int8_t mode = doc["mode"];
    Serial.print("mode: ");
    Serial.println(mode);
    switch_light(mode);

}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client_43", mqttUser, mqttPassword, NULL, NULL, NULL, NULL, true)) {
            Serial.println("connected");
            client.subscribe(mqttTopic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);

    setup_wifi();
    client.setServer(mqttServer, mqttPort); // Replace mqtt_server_ip with your MQTT broker's IP address
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    DynamicJsonDocument doc(200);
    // Your code to handle incoming MQTT messages and switch traffic lights based on the received message
}
