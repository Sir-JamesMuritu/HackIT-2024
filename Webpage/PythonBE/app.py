#!/bin/python3

from flask import Flask, jsonify, request
from flask_cors import CORS
import firebase_admin
from firebase_admin import credentials, db
from datetime import datetime

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

cred = credentials.Certificate('/mnt/c/Users/Kyo/Developer/HackIT-2024/urine-detector-system-firebase-adminsdk-ez1ls-fd45191bde.json')
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://urine-detector-system-default-rtdb.firebaseio.com'
})

# Function to fetch data from Firebase
def fetch_data_from_firebase(device_mac):
    firebase_path = f'SoilMoistureMonitoring/{device_mac}/Readings'
    try:
        data_ref = db.reference(firebase_path)
        data = data_ref.get()

        if data:
            latest_timestamp_key = max(data.keys())
            latest_data = data[latest_timestamp_key]
            return {
                'moistureLevel': latest_data.get('moistureLevel', 'N/A'),
                'timeStamp': latest_data.get('timestamp', datetime.now().strftime('%Y-%m-%d %H:%M:%S')),
                'deviceId': device_mac
            }
        else:
            return {'error': 'No data found for this device.'}
    except Exception as e:
        return {'error': str(e)}

@app.route('/get-moisture-data/<string:device_mac>', methods=['GET'])
def get_moisture_data(device_mac):
    data = fetch_data_from_firebase(device_mac)
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
