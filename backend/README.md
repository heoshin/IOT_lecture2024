### 1. `/data` (POST)

- **목적**: 하드웨어에서 보낸 센서 데이터를 수신하고 저장합니다.
- **요청 예시**:
  ```json
  POST /data
  {
      "mac_address": "00:1B:44:11:3A:B7",
      "temperature": 24.5,
      "humidity": 30,
      "brightness": 200
  }
  ```
- **응답 예시**:
  ```json
  HTTP/1.1 200 OK
  {
      "message": "데이터가 성공적으로 수신되었습니다."
  }
  ```

### 2. `/hardware` (GET)

- **목적**: 등록된 모든 하드웨어의 목록과 정보를 조회합니다.
- **요청 예시**:
  ```http
  GET /hardware
  ```
- **응답 예시**:
  ```json
  HTTP/1.1 200 OK
  [
      {
          "mac_address": "00:1B:44:11:3A:B7",
          "name": "Living Room Sensor"
      },
      {
          "mac_address": "00:1B:44:11:3A:B8",
          "name": "Kitchen Sensor"
      }
  ]
  ```

### 3. `/hardware/<mac_address>` (GET)

- **목적**: 특정 하드웨어의 센서 데이터를 조회합니다.
- **요청 예시**:
  ```http
  GET /hardware/00:1B:44:11:3A:B7
  ```
- **응답 예시**:
  ```json
  HTTP/1.1 200 OK
  {
      "name": "Living Room Sensor",
      "data": [
          {
              "temperature": 24.5,
              "humidity": 30,
              "brightness": 200,
              "timestamp": "2024-02-05T15:26:33.123Z"
          },
          {
              "temperature": 24.7,
              "humidity": 35,
              "brightness": 150,
              "timestamp": "2024-02-05T14:26:33.123Z"
          }
      ]
  }
  ```

### 4. `/hardware/<mac_address>` (POST)

- **목적**: 특정 하드웨어의 이름을 업데이트합니다.
- **요청 예시**:
  ```json
  POST /hardware/00:1B:44:11:3A:B7
  {
      "name": "New Sensor Name"
  }
  ```
- **응답 예시**:
  ```json
  HTTP/1.1 200 OK
  {
      "message": "하드웨어 이름이 성공적으로 업데이트되었습니다."
  }
  ```

### 5. `/control/<mac_address>` (GET)

- **목적**: 특정 하드웨어의 스위치 상태를 조회합니다.
- **요청 예시**:
  ```http
  GET /control/00:1B:44:11:3A:B7
  ```
- **응답 예시**:
  ```json
  HTTP/1.1 200 OK
  {
      "mac_address": "00:1B:44:11:3A:B7",
      "switch_state": true
  }
  ```

### 6. `/control/<mac_address>` (POST)

- **목적**: 특정 하드웨어의 스위치 상태를 변경합니다.
- **요청 예시**:
  ```json
  POST /control/00:1B:44:11:3A:B7
  {
      "switch_state": false
  }
  ```
- **응답 예시**:
  ```json
  HTTP/1.1 200 OK
  {
      "message": "Switch 상태가 성공적으로 업데이트되었습니다."
  }
  ```