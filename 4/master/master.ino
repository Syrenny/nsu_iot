#include <WiFi.h>
#include <HTTPClient.h>
#include <vector>
#include <ArduinoJson.h>

typedef struct TrafficLight{
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

typedef struct State
{
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

const char* ssid = "Hackspace";    //  Your Wi-Fi Name
const char* password = "you@hackspace";   // Wi-Fi Password
const char* serverAddress = "http://192.168.3.15";

int redPin = 12;
int yellowPin = 14;
int greenPin = 27;

int state_number = 0;

void switch_light(int mode)
{
    // Получаем состояния светофора из массива modes
    auto[redState, yellowState, greenState] = modes[mode];
    
    // Управляем пинами светофора в соответствии с полученными состояниями
    digitalWrite(redPin, (int8_t)redState);
    digitalWrite(yellowPin, (int8_t)yellowState);
    digitalWrite(greenPin, (int8_t)greenState);
}


void setup() {

    lastTime = millis();
    Serial.begin(115200); //Default Baudrate
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, LOW);
    Serial.println("Connecting to the Network");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected"); 
    Serial.print("IP Address of network: "); // will IP address on Serial Monitor
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void loop() 
{
    if (millis() - lastTime > timerDelay) 
    {
        if(WiFi.status() == WL_CONNECTED)
        {
            DynamicJsonDocument doc(100);
            WiFiClient client;
            HTTPClient http;

            doc["duration"] = mode_reg_2[state_number].duration;
            timerDelay = mode_reg_1[state_number].duration;
            doc["mode"] = mode_reg_2[state_number].mode;
            switch_light(mode_reg_1[state_number].mode);

            String buffer;
            serializeJson(doc, buffer);
            Serial.print("buffer: ");
            Serial.println(buffer);

            http.begin(client, serverAddress);
            http.addHeader("Content-Type", "application/json");
            int httpResponseCode = http.POST(buffer);

            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);

            http.end();
            state_number++;
            if(state_number == mode_reg_1.size())
                state_number = 0;
        }
        else
            Serial.println("WiFi Disconnected");
        lastTime = millis();
    }

}