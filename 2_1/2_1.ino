#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "IIR_WiFi";    //  Your Wi-Fi Name
const char* password = "deeprobotics";   // Wi-Fi Password

int LED = 12; 
int buttonPin = 0;
bool ledState = false;
WiFiServer server(80);

const char* serverAddress = "http://192.168.68.73";

void setup()
{
    Serial.begin(115200); //Default Baudrate
    pinMode(LED, OUTPUT);
    pinMode(buttonPin, INPUT);
    digitalWrite(LED, HIGH);
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
    if (digitalRead(buttonPin) == HIGH) 
    {
        if(WiFi.status()== WL_CONNECTED)
        {
            WiFiClient client;
            HTTPClient http;
            String serverPath = serverAddress + String(ledState ? "/LED=ON" : "/LED=OFF");
            http.begin(client, serverPath.c_str());
            int httpResponseCode = http.GET();
            if (httpResponseCode > 0) {
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                String payload = http.getString();
                Serial.println(payload);
                ledState ^= true;
            }
            else 
            {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
            // Free resources
            http.end();
        }
        else
            Serial.println("WiFi Disconnected");
        delay(1000);
    }

    WiFiClient client = server.available();
    if(client.available())
    {
        String request = client.readStringUntil('\r');
        Serial.print("Input request: ");
        Serial.println(request);
        client.flush();

        if(request.indexOf("/LED=ON") != -1)
            digitalWrite(LED, HIGH); // Turn LED ON
        if(request.indexOf("/LED=OFF") != -1)
            digitalWrite(LED, LOW); // Turn LED OFF
    }
}







