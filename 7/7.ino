#include <WiFi.h>
#include <PubSubClient.h>
#include <vector>
#include <DHT.h>
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

#define DHTPIN 27     // Пин, к которому подключен датчик DHT11
#define DHTTYPE DHT11   // Укажите тип датчика DHT

DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

const char* ssid = "MI 8";    // Your Wi-Fi Name
const char* password = "13579246";   // Wi-Fi Password

const char* mqttServer = "srv2.clusterfly.ru";
const int mqttPort = 9991;
const char* mqttUser = "user_77cc5c3b";
const char* mqttPassword = "pass_15962450";
const char* mqttTopic = "user_77cc5c3b/topic";

int redPin = 12;
int yellowPin = 14;
int greenPin = 27;

int state_number = 0;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() 
{
    delay(10);
    Serial.println("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

void callback(char* topic, byte* message, unsigned int length) 
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;
    for (int i = 0; i < length; i++) 
    {
        messageTemp += (char)message[i];
    }
    Serial.println(messageTemp);
    // Feel free to implement specific logic based on received MQTT messages
}

void reconnect() 
{
    while (!client.connected()) 
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client_40", mqttUser, mqttPassword, NULL, NULL, NULL, NULL, true)) {
            Serial.println("connected");
            client.subscribe(mqttTopic);
        } else 
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() 
{
    lastTime = millis();
    Serial.begin(115200);
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);
    dht.begin();

    setup_wifi();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) 
        reconnect();
    client.loop();

    if (millis() - lastTime > timerDelay) 
    {
        DynamicJsonDocument doc(100);
        doc["humidity"] = dht.readHumidity();
        doc["temperature"] = dht.readTemperature();

        char buffer[256];
        serializeJson(doc, buffer);
        Serial.print("buffer: ");
        Serial.println(buffer);

        client.publish(mqttTopic, buffer, true);

        lastTime = millis();
    }
}