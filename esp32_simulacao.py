import time
import random
import json
from paho.mqtt import client as mqtt_client

# Configurações MQTT
BROKER = "localhost"
PORT = 1883
TOPIC_SENSORS = "sensores"
TOPIC_CONTROL = "Controle"
CLIENT_ID = "ESP32Simulator"

# Limiares de temperatura
TEMP_UPPER_THRESHOLD = 30
TEMP_LOWER_THRESHOLD = 15

# Inicialização dos relés (0 = desligado, 1 = ligado)
relay_1 = 0
relay_2 = 0

vent = False
pico = False

# Função de callback para comandos recebidos via MQTT
def on_message(client, userdata, message):
    global relay_1, relay_2
    command = message.payload.decode()
    print(f"Comando recebido no tópico {message.topic}: {command}")
    
    if command == "ligar_ventilacao":
        vent = True
        relay_1 = 1
        relay_2 = 0
        print("Ventilação ligada.")
    elif command == "desligar_ventilacao":
        vent = False
        relay_1 = 0
        relay_2 = 0
        print("Ventilação desligada.")
    elif command == "on":
        pico = True
        relay_1 = 1
        relay_2 = 1
        print("Pico ligado.")
    elif command == "off":
        pico = False
        relay_1 = 0
        relay_2 = 0
        print("Pico desligado.")


# Função para simular leitura do sensor DHT22
def read_dht22():
    temperature = round(random.uniform(10, 40), 1)  # Simula temperatura entre 10°C e 40°C
    humidity = round(random.uniform(30, 90), 1)     # Simula umidade entre 30% e 90%
    return temperature, humidity

def connect_mqtt():
    # Configura a versão da API de callback para compatibilidade
    client = mqtt_client.Client(client_id=CLIENT_ID, userdata=None, protocol=mqtt_client.MQTTv311, transport="tcp")
    client.on_message = on_message
    client.connect(BROKER, PORT)
    return client
# Loop principal simulando o comportamento do ESP32
def main():
    global relay_1, relay_2
    
    client = connect_mqtt()
    client.loop_start()
    client.subscribe(TOPIC_CONTROL)

    print("Sistema iniciado! Simulando sensor DHT22...")
    
    while True:
        # Simula leitura do sensor
        temperature, humidity = read_dht22()
        print(f"\nTemperatura lida: {temperature}°C | Umidade lida: {humidity}%")
        
        # Lógica para controle dos relés
        if temperature < TEMP_LOWER_THRESHOLD and pico==False and vent ==False:
            relay_1 = 0
            relay_2 = 0
            print("Temperatura abaixo de 15°C. Ambos os relés desligados.")
        elif TEMP_LOWER_THRESHOLD <= temperature <= TEMP_UPPER_THRESHOLD and pico==False:
            relay_1 = 1
            relay_2 = 0
            print("Temperatura entre 15°C e 30°C. Apenas o relé 1 ligado.")
        else:
            relay_1 = 1
            relay_2 = 1
            print("Temperatura acima de 30°C. Ambos os relés ligados.")

        # Formata e publica os dados simulados via MQTT
        payload = {
            "umidade": humidity,
            "temperatura": temperature,
            "relay_1": relay_1,
            "relay_2": relay_2,
        }
        client.publish(TOPIC_SENSORS, json.dumps(payload))
        print(f"Dados enviados via MQTT: {payload}")

        # Aguarda 2 segundos antes da próxima leitura
        time.sleep(2)

if __name__ == "__main__":
    main()
