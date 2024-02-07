from flask import Flask, request, Response
import sqlite3
from datetime import datetime
import json
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

DATABASE_NAME = 'database.db'

def init_db():
    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()

    # 'hardware' 테이블 생성
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS hardware (
        mac_address TEXT PRIMARY KEY,
        name TEXT
    )
    ''')

    # 'sensor_data' 테이블 생성
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS sensor_data (
        mac_address TEXT,
        temperature REAL,
        humidity REAL,
        brightness REAL,
        timestamp DATETIME,
        FOREIGN KEY (mac_address) REFERENCES hardware(mac_address)
    )
    ''')

    # 'hardware_control' 테이블 생성
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS hardware_control (
        mac_address TEXT PRIMARY KEY,
        switch_state BOOLEAN,
        FOREIGN KEY (mac_address) REFERENCES hardware(mac_address)
    )
    ''')

    conn.commit()
    conn.close()

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json
    mac_address = data['mac_address']
    temperature = data['temperature']
    humidity = data['humidity']
    brightness = data['brightness']

    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()

    # 하드웨어 MAC 주소가 있는지 확인하고 없으면 새로운 하드웨어로 등록
    cursor.execute('SELECT mac_address FROM hardware WHERE mac_address = ?', (mac_address,))
    row = cursor.fetchone()
    if row is None:
        cursor.execute('INSERT INTO hardware (mac_address) VALUES (?)', (mac_address,))
        # 새 하드웨어에 대해 hardware_control 테이블에 switch_state를 false로 설정
        cursor.execute('INSERT INTO hardware_control (mac_address, switch_state) VALUES (?, ?)', (mac_address, False))

    # 센서 데이터를 sensor_data 테이블에 저장
    timestamp = datetime.now()
    cursor.execute('INSERT INTO sensor_data (mac_address, temperature, humidity, brightness, timestamp) VALUES (?, ?, ?, ?, ?)',
                   (mac_address, temperature, humidity, brightness, timestamp))

    conn.commit()
    conn.close()

    response = json.dumps({"message": "데이터가 성공적으로 수신되었습니다."}, ensure_ascii=False)
    return Response(response, status=200, mimetype='application/json; charset=utf-8')

@app.route('/hardware', methods=['GET'])
def get_hardware():
    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()
    cursor.execute('SELECT * FROM hardware')
    rows = cursor.fetchall()
    conn.close()

    # 결과를 JSON으로 변환
    result = [{'mac_address': row[0], 'name': row[1]} for row in rows]
    response = json.dumps(result, ensure_ascii=False)
    return Response(response, status=200, mimetype='application/json; charset=utf-8')

@app.route('/hardware/<mac_address>', methods=['GET'])
def get_sensor_data(mac_address):
    start_time = request.args.get('start_time')  # 'YYYY-MM-DD HH:MM:SS' 형식, 선택적
    end_time = request.args.get('end_time')  # 'YYYY-MM-DD HH:MM:SS' 형식, 선택적
    interval = request.args.get('interval', 'second')  # 기본값은 'second'
    limit = request.args.get('limit')  # 선택적

    # 시작점과 끝점이 설정되어 있으면 limit을 사용하지 못하게 합니다.
    if (start_time or end_time) and limit:
        return Response("Cannot use 'limit' with 'start_time' or 'end_time'", status=400)

    interval_formats = {
        'second': '%Y-%m-%d %H:%M:%S',
        'minute': '%Y-%m-%d %H:%M:00',
        'hour': '%Y-%m-%d %H:00:00',
        'day': '%Y-%m-%d 00:00:00',
        'month': '%Y-%m-01 00:00:00',
        'year': '%Y-01-01 00:00:00'
    }

    if interval not in interval_formats:
        return Response("Invalid interval", status=400)

    interval_format = interval_formats[interval]

    # 끝 시간이 없으면 현재 시간을 사용합니다.
    end_time = end_time or datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()

    # 데이터 집계 쿼리
    query = f'''
    SELECT 
        strftime('{interval_format}', timestamp) as interval,
        AVG(temperature) as avg_temperature,
        AVG(humidity) as avg_humidity,
        AVG(brightness) as avg_brightness
    FROM sensor_data
    WHERE 
        mac_address = ?
    '''
    params = [mac_address]

    # 시작 시간과 끝 시간 조건을 쿼리에 추가
    if start_time:
        query += ' AND timestamp >= ?'
        params.append(start_time)
    query += ' AND timestamp <= ?'
    params.append(end_time)

    query += f' GROUP BY interval ORDER BY interval ASC'

    # limit이 설정되어 있으면 쿼리에 추가합니다.
    if limit:
        query += ' LIMIT ?'
        params.append(limit)

    cursor.execute(query, params)
    rows = cursor.fetchall()
    conn.close()

    # 결과를 JSON으로 변환
    result = {
        'mac_address': mac_address,
        'data': [{'timestamp': row[0], 'temperature': round(row[1], 1), 'humidity': round(row[2], 1), 'brightness': int(row[3])} for row in rows]
    }

    response = json.dumps(result, ensure_ascii=False)
    return Response(response, status=200, mimetype='application/json; charset=utf-8')

@app.route('/hardware/<mac_address>', methods=['POST'])
def update_hardware_name(mac_address):
    data = request.json
    name = data.get('name')
    
    if not name:
        response = json.dumps({"message": "이름을 제공해야 합니다."}, ensure_ascii=False)
        return Response(response, status=400, mimetype='application/json; charset=utf-8')

    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()

    cursor.execute('UPDATE hardware SET name = ? WHERE mac_address = ?', (name, mac_address))
    affected_rows = cursor.rowcount
    conn.commit()
    conn.close()

    if affected_rows == 0:
        response = json.dumps({"message": "해당 MAC 주소를 가진 하드웨어가 없습니다."}, ensure_ascii=False)
        return Response(response, status=404, mimetype='application/json; charset=utf-8')

    response = json.dumps({"message": "하드웨어 이름이 성공적으로 업데이트되었습니다."}, ensure_ascii=False)
    return Response(response, status=200, mimetype='application/json; charset=utf-8')

if __name__ == '__main__':
    init_db()
    app.run(host="0.0.0.0", port=82, debug=True)
