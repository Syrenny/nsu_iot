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

typedef struct State {
    int8_t mode;
    int duration;
} State;

std::vector<State> mode_reg_1 = {
    {0, 6000},
    {1, 1000},
    {0, 1000},
    {1, 1000},
    {0, 1000},
    {1, 1000},
    {0, 1000}, 
    {2, 3000},
    {3, 6000},
    {3, 1000},
    {3, 1000},
    {3, 1000},
    {3, 1000},
    {3, 1000},
    {3, 1000},
    {4, 3000}
};

std::vector<State> mode_unreg = {
    {2, 1000},  
    {1, 1000}
};

std::vector<State> mode_reg_2 = {
    {3, 6000},
    {3, 1000},
    {3, 1000},
    {3, 1000},
    {3, 1000},
    {3, 1000},
    {3, 1000},
    {4, 3000},
    {0, 6000},
    {1, 1000},
    {0, 1000},
    {1, 1000},
    {0, 1000},
    {1, 1000},
    {0, 1000},
    {2, 3000}
};

unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

const char* ssid = "Hackspace";    // Your Wi-Fi Name
const char* password = "you@hackspace";   // Wi-Fi Password

const char* mqttServer = "srv2.clusterfly.ru";
const int mqttPort = 9991;
const char* mqttUser = "user_1f40dd56";
const char* mqttPassword = "FYXS8AV-U-pgH";
const char* mqttTopic = "user_1f40dd56/temp";

int redPin = 12;
int yellowPin = 14;
int greenPin = 27;

int state_number = 0;
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
    // Feel free to implement specific logic based on received MQTT messages
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client_42", mqttUser, mqttPassword, NULL, NULL, NULL, NULL, true)) {
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
    lastTime = millis();
    Serial.begin(115200);
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);

    setup_wifi();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    if (millis() - lastTime > timerDelay) {
        DynamicJsonDocument doc(100);
        doc["duration"] = mode_reg_2[state_number].duration;
        timerDelay = mode_reg_1[state_number].duration;
        doc["mode"] = mode_reg_2[state_number].mode;
        switch_light(mode_reg_1[state_number].mode);

        char buffer[256];
        serializeJson(doc, buffer);
        Serial.print("buffer: ");
        Serial.println(buffer);

        client.publish(mqttTopic, buffer, true);

        state_number++;
        if(state_number == mode_reg_1.size())
            state_number = 0;

        lastTime = millis();
    }
}
