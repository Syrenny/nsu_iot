#include <Esp32WifiManager.h>

const char* ssid = "Hackspace";    //  Your Wi-Fi Name
const char* password = "you@hackspace";   // Wi-Fi Password
int LED = 12;   //
WiFiServer server(80);

void setup()
{
  Serial.begin(115200); //Default Baudrate
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.print("Connecting to the Newtork");
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
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: http://"); // Will print IP address in URL format
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop()
{
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  while(!client.available())
  {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  int value = LOW;
  if(request.indexOf("/LED=ON") != -1)
  {
    digitalWrite(LED, HIGH); // Turn LED ON
    value = HIGH;
  }
  if(request.indexOf("/LED=OFF") != -1)
  {
    digitalWrite(LED, LOW); // Turn LED OFF
    value = LOW;
  }
}