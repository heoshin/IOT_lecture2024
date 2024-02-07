#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <Arduino_JSON.h>
#include <JSON.h>
#include <JSONVar.h>

const String HARDWARE_ID = WiFi.macAddress();

// 센서 세팅
DHT dht(D2, DHT22);                    // 온습도 센서
#define BRIGHTNESS_BASE_RESISTOR 10000 // 조도 센서 기준저항

// 와이파이 세팅
#define WIFI_SSID "TEST_SSID"
#define WIFI_PASSWORD "test1234"
WiFiClient wifiClient;

// API 정보
#define API_URL "http://pcs.pah.kr:82"
#define API_URL_ADD_DATA API_URL "/data"
#define API_URL_GET_SWITCH_STATUS API_URL "/control"

void setup()
{
  Serial.begin(115200);
  // 디버깅 정보 출력
  const auto chipId = ESP.getChipId();
  const auto chipId2 = CHIPID;
  Serial.println("====INFO====");
  Serial.printf("chipid: %08X\n", chipId);
  Serial.printf("chipid2: %08X\n", chipId2);
  Serial.printf("HARDWARE_ID: %u", HARDWARE_ID);
  Serial.printf("macAddress: %s\n", WiFi.macAddress());
  Serial.printf("build timestamp: %s %s\n", __DATE__, __TIME__);
  Serial.println("============");

  // 온습도 센서 세팅
  dht.begin();

  // LED 세팅
  pinMode(LED_BUILTIN, OUTPUT);

  // wifi 연결
  Serial.printf("SSID: %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // wifi 연결 완료까지 대기
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.printf("Connected! IP address: %s", WiFi.localIP());
}

void loop()
{
  // WIFI 재연결 처리
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("[ERROR] Wifi is not connected, Try reconnect");
    while (!WiFi.reconnect())
    {
      delay(500);
      Serial.print('.');
    }
    Serial.println();
    Serial.printf("Connected! IP address: %s", WiFi.localIP());
  }

  // 1000ms 마다 센서 데이터 전송
  {
    static unsigned long previousMillis = 0; // 마지막으로 작업을 수행한 시점을 저장
    const unsigned long interval = 1000;
    unsigned long currentMillis = millis();
    while (currentMillis - previousMillis < interval) // 마지막 작업시간으로부터 1000ms 이상 경과하지 않은경우
    {
      currentMillis = millis();
    }
    previousMillis = currentMillis;
  }

  // 센서 데이터 전송
  SendSensorData(wifiClient);
  
  // LED 제어
  const auto isSwitchOn = IsSwitchOnFromServer(wifiClient, HARDWARE_ID);
  Serial.printf("isSwitchOn: %d", isSwitchOn);
  if (isSwitchOn)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

bool SendSensorData(WiFiClient& wifiClient)
{
  // 습도와 온도 읽기
  const float humidity = dht.readHumidity();
  const float temperature = dht.readTemperature();

  // 조도 센서 읽기
  const int sensorValue = analogRead(A0);
  int brightness = 0;
  // 센서 오류 처리
  if (!calculateBrightness(sensorValue, BRIGHTNESS_BASE_RESISTOR, brightness))
  {
    Serial.println("Brightness sensor error");
    brightness = 0; // 오류 시 조도값을 0으로 설정
  }
  Serial.printf("Humidity: %f%%, Temperature: %f°C, Brightness: %f Ohm\n", humidity, temperature, brightness);

  // 서버에 전송 할 json 데이터 구성
  JSONVar jsonData;
  jsonData["mac_address"] = HARDWARE_ID;
  jsonData["temperature"] = temperature;
  jsonData["humidity"] = humidity;
  jsonData["brightness"] = brightness;

  // 서버로 데이터 전송
  return HttpPost(wifiClient, API_URL_ADD_DATA, jsonData);
}

int IsSwitchOnFromServer(WiFiClient &wifiClient, const String& id)
{
  const auto& requestUrl = String{API_URL_GET_SWITCH_STATUS} + "/" + id;
  auto jsonData = HttpGet(wifiClient, requestUrl.c_str());
  if (jsonData.hasOwnProperty("switch_state"))
  {
    const bool isSwitchOn = jsonData["switch_state"];
    return isSwitchOn == 1;
  }
  else
  {
    Serial.printf("[ERROR] switch_status data not exist, jsonData: %s", jsonData);
    return -1;
  }
}

bool HttpPost(WiFiClient &wifiClient, const char *apiUrl, const JSONVar &jsonData)
{
  HTTPClient http;

  Serial.print("====[HTTP] POST====\n");
  http.begin(wifiClient, apiUrl);
  http.addHeader("Content-Type", "application/json");
  const auto &jsonString = JSON.stringify(jsonData);
  Serial.println("====POST body====");
  Serial.println(jsonString);
  Serial.println("=================");
  const int httpCode = http.POST(jsonString);

  if (httpCode > 0)
  {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK)
    {
      const String &payload = http.getString();
      Serial.println("===received payload===");
      Serial.println(payload);
      Serial.println("======================");
    }
  }
  else
  {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return httpCode == HTTP_CODE_OK;
}

inline JSONVar HttpGet(WiFiClient &wifiClient, const char *apiUrl)
{
  HTTPClient http;
  String payload;
  Serial.print("====[HTTP] GET====\n");
  http.begin(wifiClient, apiUrl); // HTTP 시작과 동시에 주어진 URL로 연결합니다.

  int httpCode = http.GET(); // HTTP GET 요청을 보냅니다.

  if (httpCode > 0) // HTTP 요청이 성공적으로 이루어졌는지 확인합니다.
  {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) // 서버로부터 200 OK 응답을 받았는지 확인합니다.
    {
      payload = http.getString(); // 응답 페이로드를 받습니다.
      Serial.println("===received payload===");
      Serial.println(payload);
      Serial.println("======================");
    }
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();          // HTTP 연결을 종료합니다.
  return JSON.parse(payload); // 파싱된 JSON 객체를 반환합니다.
}

bool calculateBrightness(const int sensorValue, const int baseResistor, int &brightness)
{
  if (sensorValue < 0 || sensorValue > 1023)
  {
    return false; // 센서 값 범위 오류
  }
  // 조도 계산 (옴으로 변환)
  brightness = sensorValue * (float(baseResistor) / (1023 - sensorValue));
  return true;
}
