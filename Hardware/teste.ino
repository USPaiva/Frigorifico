#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>  // Biblioteca DHTesp

// Configurações do Wi-Fi e MQTT
const char* ssid = "UnivapWifi";
const char* password = "universidade";
const char* mqtt_server = "jogosip.ddns.net";
const char* mqtt_publish_topic = "sensores";
const char* mqtt_command_topic = "Controle";

// Configuração do DHT22
#define DHTPIN 26            // Pin onde o DHT22 está conectado
DHTesp dht;                // Cria uma instância da classe DHTesp

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Função para conexão WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Função callback para mensagens recebidas via MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Comando recebido: [");
  Serial.print(topic);
  Serial.print("] ");
  String command;
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  Serial.println(command);

  // Interprete e execute comandos recebidos
  if (command == "ligar_ventilacao") {
    Serial.println("Ventilação ligada");
  } else if (command == "desligar_ventilacao") {
    Serial.println("Ventilação desligada");
  }
}

// Reconecta ao MQTT caso a conexão caia
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando se conectar ao MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado");
      client.subscribe(mqtt_command_topic);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  dht.setup(DHTPIN, DHTesp::DHT22);  // Configura o DHT22
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Envia a cada 5 segundos
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Lê a temperatura e umidade
    float h = dht.getHumidity();
    float t = dht.getTemperature();

    // Verifica se a leitura foi bem-sucedida
    if (isnan(h) || isnan(t)) {
      Serial.println("Falha na leitura do sensor DHT22!");
      return;
    }

    // Formata e envia os dados via MQTT
    String payload = "{";
    payload += "\"umidade\": ";
    payload += String(h);
    payload += ", \"temperatura\": ";
    payload += String(t);
    payload += "}";

    payload.toCharArray(msg, 50);
    client.publish(mqtt_publish_topic, msg);
    Serial.print("Dados enviados: ");
    Serial.println(msg);
  }
}
