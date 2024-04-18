#include <WiFi.h>
#include <HTTPClient.h>

bool States_1[][3] = {
    //red yellow green
    {false, false, true},
    {false, false, false},
    {false, false, true},
    {false, false, false},
    {false, false, true},
    {false, false, false},
    {false, false, true},
    {false, true, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, true, false}
};

bool States_2[][3] = {
    //red yellow green
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, false, false},
    {true, true, false},
    {false, false, true},
    {false, false, false},
    {false, false, true},
    {false, false, false},
    {false, false, true},
    {false, false, false},
    {false, false, true},
    {false, true, false}
};

const int Duration[] = {6, 1, 1, 1, 1, 1, 1, 3, 6, 1, 1, 1, 1, 1, 1, 3};

const char* ssid = "Hackspace";    //  Your Wi-Fi Name
const char* password = "you@hackspace";   // Wi-Fi Password

int LED = 14; 
bool ledState = false;
WiFiServer server(80);

unsigned long lastTime = 0;
unsigned long timerDelay = 200;

const char* serverAddress = "http://192.168.3.5";
int state = 0

void setup()
{
    Serial.begin(115200); //Default Baudrate
    pinMode(LED, OUTPUT);
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
    if(WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;
        HTTPClient http;
        String serverPath = serverAddress + String(ledState ? "/LED=ON" : "/LED=OFF");
        http.begin(client, serverPath.c_str());
        int httpResponseCode = http.GET();
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        ledState ^= true;
        http.end();
    }
    else
        Serial.println("WiFi Disconnected");


    if(millis() - lastTime > )









    if (millis() - lastTime > timerDelay) 
    {
        if(WiFi.status() == WL_CONNECTED)
        {
            WiFiClient client;
            HTTPClient http;
            String serverPath = serverAddress + String(ledState ? "/LED=ON" : "/LED=OFF");
            http.begin(client, serverPath.c_str());
            int httpResponseCode = http.GET();
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            ledState ^= true;
            http.end();
        }
        else
            Serial.println("WiFi Disconnected");
        lastTime = millis();
    }

    WiFiClient client = server.available();
    String request = client.readStringUntil('\r');
    client.flush();

    if(request.indexOf("/LED=ON") != -1)
        digitalWrite(LED, HIGH); // Turn LED ON
    if(request.indexOf("/LED=OFF") != -1)
        digitalWrite(LED, LOW); // Turn LED OFF
}







