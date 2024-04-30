#include <WiFi.h>
#include <vector>
#include <ArduinoJson.h>

typedef struct TrafficLight
{
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

const char* ssid = "Hackspace";    //  Your Wi-Fi Name
const char* password = "you@hackspace";   // Wi-Fi Password
WiFiServer server(80);

void switch_light(int mode)
{
    // Получаем состояния светофора из массива modes
    auto[redState, yellowState, greenState] = modes[mode];
    
    // Управляем пинами светофора в соответствии с полученными состояниями
    digitalWrite(redPin, (int8_t)redState);
    digitalWrite(yellowPin, (int8_t)yellowState);
    digitalWrite(greenPin, (int8_t)greenState);
}

String handleClientRequest(WiFiClient client) 
{
    if(!client.find("{"))
        return String();
    String json = client.readStringUntil('}');
    return String("{" + json + "}");
}

String extractJsonFromBody(String request) 
{
    // Находим начало и конец JSON в теле запроса
    int jsonStart = request.indexOf('{');
    int jsonEnd = request.lastIndexOf('}');

    // Если JSON найден, извлекаем его
    if (jsonStart != -1 && jsonEnd != -1)
        return request.substring(jsonStart, jsonEnd + 1);
    else 
        return String();
}

void setup()
{
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
    server.begin();  // Starts the Server
    Serial.println("Server started");
    Serial.print("IP Address of network: "); // will IP address on Serial Monitor
    Serial.print(WiFi.localIP());
    Serial.println("/");
}
void loop() 
{
    if(WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client = server.available();
        DynamicJsonDocument doc(200);
        String json = handleClientRequest(client);
        if(!json.isEmpty())
        {
            Serial.print("json: ");
            Serial.println(json);
        }
        DeserializationError error = deserializeJson(doc, json);
        client.flush();
        if (error) 
            return;
        Serial.print("message: ");
        Serial.println(json);
        int8_t mode = doc["mode"];
        Serial.print("mode: ");
        Serial.println(mode);
        switch_light(mode);
    }
    else
        Serial.println("WiFi Disconnected");
}

