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
    result = db_manager.fetch_data(measurement="Frigorifico")
    serial_data = db_manager.parse_data(result)
    return jsonify(serial_data)

@app.route('/pico', methods=['POST'])
def toggle_pico():
    data = request.json
    action = data.get('action')

    if action == 'on':
        # Envie um comando MQTT para ligar o LED
        mqtt.single("Controle", "on", hostname=mqtt_host,port=mqtt_port)
        return jsonify({"status": "Modo de pico ligado"}), 200
    elif action == 'off':
        # Envie um comando MQTT para desligar o LED
        mqtt.single("Controle", "off", hostname=mqtt_host,port=mqtt_port)
        return jsonify({"status": "Modo de pico desligado"}), 200
    else:
        return jsonify({"error": "Comando inválido"}), 400

@app.route('/ventilador', methods=['POST'])
def ventilador():
    data = request.json
    action = data.get('action')

    if action == 'on':
        # Envie um comando MQTT para ligar o LED
        mqtt.single("Controle", "ligar_ventilacao", hostname=mqtt_host,port=mqtt_port)
        return jsonify({"status": "ventilador ligado"}), 200
    elif action == 'off':
        # Envie um comando MQTT para desligar o LED
        mqtt.single("Controle", "desligar_ventilacao", hostname=mqtt_host,port=mqtt_port)
        return jsonify({"status": "ventilador desligado"}), 200
    else:
        return jsonify({"error": "Comando inválido"}), 400

@app.route('/set-temperature-limit', methods=['POST'])
def set_temperature_limit():
    data = request.json
    max_temp = data.get('temperature')

    if max_temp is not None:
        try:
            # Enviar o limite de temperatura via MQTT (se necessário)
            mqtt.single("Controle", f"limite_temp:{max_temp}", hostname=mqtt_host, port=mqtt_port)
            return jsonify({"status": "Limite de temperatura definido", "temperature": max_temp}), 200
        except Exception as e:
            return jsonify({"error": "Falha ao enviar comando MQTT", "details": str(e)}), 500
    else:
        return jsonify({"error": "Limite de temperatura não fornecido"}), 400

@app.route('/set-pico-schedule', methods=['POST'])
def set_pico_schedule():
    data = request.json
    on_time = data.get('on')
    off_time = data.get('off')

    if on_time and off_time:
        try:
            # Enviar os horários via MQTT (se necessário)
            mqtt.single("Controle", f"modo_turbo:on:{on_time}:off:{off_time}", hostname=mqtt_host, port=mqtt_port)
            return jsonify({"status": "Horários do modo turbo definidos", "on": on_time, "off": off_time}), 200
        except Exception as e:
            return jsonify({"error": "Falha ao enviar comando MQTT", "details": str(e)}), 500
    else:
        return jsonify({"error": "Horários não fornecidos"}), 400