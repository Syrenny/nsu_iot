#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

const char* ssid = "Hackspace";
const char* password = "you@hackspace";

#define DHTPIN 27     // Пин, к которому подключен датчик DHT11
#define DHTTYPE DHT11   // Укажите тип датчика DHT

DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(1000);
        Serial.println("...");
    }

    Serial.println("Connected to WiFi");

    Serial.print("IP Address of network: "); // will IP address on Serial Monitor
    Serial.println(WiFi.localIP());   

    dht.begin();

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    WiFiClient client = server.available();
    if (!client)
        return;

    while (!client.available())
        delay(1);


    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("/temperature") != -1) 
    {
        float temperature = dht.readTemperature();

        DynamicJsonDocument doc(128);
        doc["temperature"] = temperature;

        String response;
        serializeJson(doc, response);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(response);
    } 
    else if (request.indexOf("/humidity") != -1) 
    {
        float humidity = dht.readHumidity();

        DynamicJsonDocument doc(128);
        doc["humidity"] = humidity;

        String response;
        serializeJson(doc, response);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(response);
    } 
    else 
    {
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Not found");
    }
    delay(1);
    client.stop();
}
