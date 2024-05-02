#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YourWiFiNetwork";     // Название вашей Wi-Fi сети
const char* password = "YourWiFiPassword"; // Пароль для вашей Wi-Fi сети

WebServer server(80);

const int ledPin = 2;  // Пин для светодиода

bool isInSetupMode = false; // Флаг для отслеживания режима настройки

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Проверка, запускается ли устройство в режиме настройки
  if (digitalRead(BUTTON_PIN) == HIGH) {
    isInSetupMode = true;
    setupMode();
  } else {
    // Иначе, устройство запускается в нормальном режиме работы
    normalMode();
  }
}

void loop() {
  if (isInSetupMode) {
    server.handleClient();
    // Дополнительные действия в режиме настройки
  } else {
    // Действия в нормальном режиме работы
  }
}

void setupMode() {
  digitalWrite(ledPin, HIGH); // Включаем светодиод в режиме настройки
  
  Serial.begin(115200);
  Serial.println("Entering setup mode");

  // Установка точки доступа Wi-Fi для настройки
  WiFi.softAP("ESP32-Setup", "password");

  // Запуск WEB-сервера для настройки
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", "<h1>Setup Mode</h1><p>Please configure your device.</p>");
  });
  server.begin();
  Serial.println("HTTP server started");

  // Здесь можно добавить код для выбора Wi-Fi сети и пароля
}

void normalMode() {
  digitalWrite(ledPin, LOW); // Выключаем светодиод в нормальном режиме
  
  Serial.begin(115200);
  Serial.println("Entering normal mode");

  // Подключение к Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Действия в нормальном режиме работы
}
