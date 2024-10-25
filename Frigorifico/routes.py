import os
from flask import Flask, render_template, jsonify, request, url_for, redirect
from Frigorifico.Banco.InfluxDBManager import InfluxDBManager
from datetime import datetime
import pandas as pd
from Frigorifico import app
import json
import paho.mqtt.publish as mqtt 
from Frigorifico.Config.Config import mqtt_port,mqtt_host

# Instancia o gerenciador do InfluxDB
db_manager = InfluxDBManager()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data', methods=['GET'])
def get_data():
    result = db_manager.fetch_data(measurement="Satélite")
    serial_data = db_manager.parse_data(result)
    return jsonify(serial_data)

@app.route('/led-control', methods=['POST'])
def led_control():
    data = request.json
    action = data.get('action')

    if action == 'on':
        # Envie um comando MQTT para ligar o LED
        mqtt.single("Controle", "on", hostname=mqtt_host,port=mqtt_port)
        return jsonify({"status": "LED ligado"}), 200
    elif action == 'off':
        # Envie um comando MQTT para desligar o LED
        mqtt.single("Controle", "off", hostname=mqtt_host,port=mqtt_port)
        return jsonify({"status": "LED desligado"}), 200
    else:
        return jsonify({"error": "Comando inválido"}), 400
