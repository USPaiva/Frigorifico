import json
import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
import warnings
from datetime import datetime
warnings.filterwarnings("ignore", category=DeprecationWarning)
from Config.Config import influxdb_url,influxdb_token,influxdb_org,influxdb_bucket,mqtt_port,mqtt_host
############################################################################################################################################

def influxdb(msg):
    try:
        campos_json = json.loads(msg.payload.decode('utf-8'))
        # Grava cada ponto no InfluxDB
        with InfluxDBClient(url=influxdb_url, token=influxdb_token, org=influxdb_org) as client:
            write_api = client.write_api(write_options=SYNCHRONOUS) 
                # Cria um ponto para cada campo no JSON
            point = Point("Frigorifico") \
                .tag("id", id) \
                .field("Umid", campos_json["umidade"]) \
                .field("Temp", campos_json["temperatura"]) \
                .field("Vent1", campos_json["relay_1"]) \
                .field("Vent2", campos_json["relay_2"]) \
                .time(datetime.utcnow(), WritePrecision.NS)
            write_api.write(bucket=influxdb_bucket, record=point)
        print(f'Registro adicionado: {point}')
        print(f"[RECEBIDO] Mensagem salva no InfluxDB: {campos_json}")
    except Exception as e:
        print(f"[ERRO] Um erro inesperado ocorreu: {e}")

############################################################################################################################

##MQTT

def on_connect(client, userdata, flags, rc):
    print("entrou no on_connect")
    if rc == 0:
        print('Conectado ao broker MQTT')
        client.subscribe('sensores')
    else:
        print(f'Falha na conexão, código: {rc}')

def on_message(client, userdata, msg):
    print("entrou no on_message")
    id = msg.topic
    
    MensagemRecebida = str(msg.payload)

    print("[MSG RECEBIDA] Topico: " + msg.topic + " / Mensagem: " + MensagemRecebida)
    
    try:
        influxdb(msg)
    except json.JSONDecodeError:
        print(f"[ERRO] Falha ao decodificar a mensagem: {msg.payload.decode('utf-8')}")
    except Exception as e:
        print(f"[ERRO] Um erro inesperado ocorreu: {e}")

def on_log(client, userdata, level, buf):
    print(f"Log: {buf}")


##############################################################################
# Programa principal
if __name__ == "__main__":
    print('Ligação entre broker MQTT e InfluxDB')
    
    cliente_mqtt = mqtt.Client()
    cliente_mqtt.on_connect = on_connect
    cliente_mqtt.on_message = on_message
    # Conectar ao broker MQTT
    result = cliente_mqtt.connect(host=mqtt_host,port=mqtt_port, keepalive=60)
    cliente_mqtt.on_log = on_log
    # Inicia o loop do cliente MQTT
    cliente_mqtt.loop_start()
    try:
        while True:
            pass  # Mantém o loop ativo
    except KeyboardInterrupt:
        print("Encerrando...")
        cliente_mqtt.loop_stop()
