#include <DHT.h>

#define DHTPIN D2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int BASE_RESISTOR = 10000; // 기준저항 10k옴

void setup()
{
  Serial.begin(115200);
  Serial.print("timestamp: ");
  Serial.println(__TIMESTAMP__);

  dht.begin();
}

void loop()
{
  // 습도와 온도 읽기
  const float humidity = dht.readHumidity();
  const float temperature = dht.readTemperature();

  // 조도 센서 읽기
  const int sensorValue = analogRead(A0);
  int brightness = calculateBrightness(sensorValue, BASE_RESISTOR);

  // 센서 오류 처리
  if (brightness == -1)
  {
    Serial.println("Brightness sensor error");
    brightness = 0; // 오류 시 조도값을 0으로 설정
  }

  // 센서 값 출력
  Serial.printf("Humidity: %f%%, Temperature: %f°C, Brightness: %d Ohm\n", humidity, temperature, brightness);

  // 일정 시간 대기
  delay(2000); // 2초 대기
}

double calculateBrightness(const int sensorValue, const int baseResistor)
{
  if (sensorValue < 0 || sensorValue > 1023)
  {
    return -1; // 센서 값 범위 오류
  }
  // 조도 계산 (옴으로 변환)
  return sensorValue * (double(baseResistor) / (1023 - sensorValue));
}
