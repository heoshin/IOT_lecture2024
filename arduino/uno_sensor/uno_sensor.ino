#include <WiFiS3.h>
#include <DHT.h>
#include <Arduino_JSON.h>

#define DHTPIN 2     // DHT 센서가 연결된 핀 번호
#define DHTTYPE DHT22   // DHT 22 (AM2302), DHT11 등 센서 타입 설정
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "TEST_SSID";         // 네트워크 SSID (이름)
char pass[] = "test1234";    // 네트워크 비밀번호

WiFiClient client;

// 서버 주소
char server[] = "pcs.pah.kr";
int port = 82; // 서버 포트

const unsigned long postingInterval = 10L * 1000L; // 10초마다 데이터 전송
unsigned long lastConnectionTime = 0;   

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.print("Connected to wifi. IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from WiFi, attempting reconnection.");
    WiFi.begin(ssid, pass);
    delay(10000);
  }

  if (millis() - lastConnectionTime > postingInterval) {
    sendSensorData();
    checkSwitchStatus();
    lastConnectionTime = millis();
  }
}

void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int sensorValue = analogRead(A0);
  int brightness = sensorValue;  // 조도 센서 계산 로직이 필요할 수 있습니다.

  Serial.println("Sending sensor data to server...");

  uint8_t mac[6];
  WiFi.macAddress(mac);
  String macStr = macToString(mac);
  
  JSONVar jsonData;
  jsonData["mac_address"] = macStr;
  jsonData["temperature"] = temperature;
  jsonData["humidity"] = humidity;
  jsonData["brightness"] = brightness;

  if (client.connect(server, port)) {
    client.println("POST /data HTTP/1.1");
    client.println("Host: pcs.pah.kr");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JSON.stringify(jsonData).length());
    client.println();
    client.print(JSON.stringify(jsonData));

    // Read server response
    delay(1000);
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  }
  client.stop();
}

void checkSwitchStatus() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String macStr = macToString(mac);
  String path = "/control/" + macStr;

  if (client.connect(server, port)) {
    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: pcs.pah.kr");
    client.println("Connection: close");
    client.println();

    // Read server response
    String line = client.readStringUntil('\n');
    if (line.startsWith("{\"state\":\"ON\"}")) {
      digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED on
    } else if (line.startsWith("{\"state\":\"OFF\"}")) {
      digitalWrite(LED_BUILTIN, LOW);    // Turn the LED off
    }

    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  }
  client.stop();
}

String macToString(uint8_t* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}