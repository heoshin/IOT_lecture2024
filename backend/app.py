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
    hardware_id = data['hardware_id']
    temperature = data['temperature']
    humidity = data['humidity']
    brightness = data['brightness']

    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()

    # 하드웨어ID가 있는지 확인하고 없으면 새 UUID를 생성
    cursor.execute('SELECT uuid FROM hardware WHERE hardware_id = ?', (hardware_id,))
    row = cursor.fetchone()
    if row is None:
        new_uuid = str(uuid.uuid4())
        cursor.execute('INSERT INTO hardware (hardware_id, uuid) VALUES (?, ?)', (hardware_id, new_uuid))
    else:
        new_uuid = row[0]

    # 센서 데이터를 sensor_data 테이블에 저장
    timestamp = datetime.now()
    cursor.execute('INSERT INTO sensor_data (uuid, temperature, humidity, brightness, timestamp) VALUES (?, ?, ?, ?, ?)',
                   (new_uuid, temperature, humidity, brightness, timestamp))

    conn.commit()
    conn.close()

    return jsonify({"message": "데이터가 성공적으로 수신되었습니다."}), 200

@app.route('/hardware', methods=['GET'])
def get_hardware():
    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()
    cursor.execute('SELECT * FROM hardware')
    rows = cursor.fetchall()
    conn.close()

    # 결과를 JSON으로 변환
    result = [{'hardware_id': row[0], 'uuid': row[1], 'name': row[2]} for row in rows]
    return jsonify(result), 200

@app.route('/hardware/<hardware_uuid>', methods=['GET'])
def get_sensor_data(hardware_uuid):
    limit = request.args.get('limit', 10)
    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()

    # 첫 번째 테이블에서 하드웨어 이름 조회
    cursor.execute('SELECT name FROM hardware WHERE uuid = ?', (hardware_uuid,))
    hardware_name = cursor.fetchone()
    
    if not hardware_name:
        return jsonify({"message": "해당 UUID를 가진 하드웨어가 없습니다."}), 404

    # 두 번째 테이블에서 센서 데이터 조회, 최신 데이터가 먼저 나오도록 내림차순 정렬
    cursor.execute('SELECT temperature, humidity, brightness, timestamp FROM sensor_data WHERE uuid = ? ORDER BY timestamp DESC LIMIT ?', (hardware_uuid, limit))
    rows = cursor.fetchall()
    conn.close()

    # 결과를 JSON으로 변환
    result = {
        'name': hardware_name[0],
        'data': [{'temperature': row[0], 'humidity': row[1], 'brightness': row[2], 'timestamp': row[3]} for row in rows]
    }
    return jsonify(result), 200

@app.route('/hardware/<hardware_uuid>', methods=['POST'])
def update_hardware_name(hardware_uuid):
    data = request.json
    name = data.get('name')
    
    if not name:
        return jsonify({"message": "이름을 제공해야 합니다."}), 400

    conn = sqlite3.connect(DATABASE_NAME)
    cursor = conn.cursor()

    cursor.execute('UPDATE hardware SET name = ? WHERE uuid = ?', (name, hardware_uuid))
    affected_rows = cursor.rowcount
    conn.commit()
    conn.close()

    if affected_rows == 0:
        return jsonify({"message": "해당 UUID를 가진 하드웨어가 없습니다."}), 404

    return jsonify({"message": "하드웨어 이름이 성공적으로 업데이트되었습니다."}), 200

if __name__ == '__main__':
    init_db()
    app.run(host="0.0.0.0", port=81, debug=True)
