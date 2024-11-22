/*


testar tudo junto e ver se a leitura de umidade também esta funcionando


*/
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Configurações do Wi-Fi e MQTT
const char* ssid = "UnivapWifi";
const char* password = "universidade";
const char* mqtt_server = "jogosip.ddns.net";
const char* mqtt_publish_topic = "sensores";
const char* mqtt_command_topic = "Controle";



#define DHT_SENSOR_PIN 23 // ESP32 pin GPIO23 connected to DHT22
#define RELAY_1_PIN    18 // ESP32 pin GPIO18 connected to relay 1
#define RELAY_2_PIN    19 // ESP32 pin GPIO19 connected to relay 2

#define DHT_SENSOR_TYPE DHT22

#define TEMP_UPPER_THRESHOLD  30 // Upper temperature threshold
#define TEMP_LOWER_THRESHOLD  15 // Lower temperature threshold

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);


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
	  digitalWrite(RELAY_1_PIN, HIGH);
    digitalWrite(RELAY_2_PIN, HIGH);
  } else if (command == "desligar_ventilacao") {
    Serial.println("Ventilação desligada");
  	digitalWrite(RELAY_1_PIN, LOW);
  	digitalWrite(RELAY_2_PIN, LOW);
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
  Serial.begin(9600); // Initialize serial
  dht_sensor.begin(); // Initialize the DHT sensor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // Setup relay pins
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, LOW); // Ensure relays are off initially
  digitalWrite(RELAY_2_PIN, LOW);

  Serial.println("Sistema iniciado!");
  Serial.println("Inicializando o sensor DHT22...");
  delay(2000); // Aguarda o sensor estabilizar
  Serial.println("Pronto para leituras!");
}

void loop() {
  Serial.println("\nLendo a temperatura e umidade...");
  float temperature = dht_sensor.readTemperature();  // Read temperature in Celsius
  float Humidity = dht_sensor.readHumidity();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (isnan(temperature)) {
    Serial.println("Erro: Falha ao ler o sensor DHT!");
  } else {
    Serial.print("Temperatura lida: ");
    Serial.print(temperature);
    Serial.println("°C");

    if (temperature < TEMP_LOWER_THRESHOLD) {
      Serial.println("Temperatura abaixo de 15°C. Ambos os relés desligados.");
      digitalWrite(RELAY_1_PIN, LOW);
      digitalWrite(RELAY_2_PIN, LOW);
    } else if (temperature >= TEMP_LOWER_THRESHOLD && temperature <= TEMP_UPPER_THRESHOLD) {
      Serial.println("Temperatura entre 15°C e 30°C. Apenas o relé 1 ligado.");
      digitalWrite(RELAY_1_PIN, HIGH);
      digitalWrite(RELAY_2_PIN, LOW);
    } else {
      Serial.println("Temperatura acima de 30°C. Ambos os relés ligados.");
      digitalWrite(RELAY_1_PIN, HIGH);
      digitalWrite(RELAY_2_PIN, HIGH);
    }
	
	// Formata e envia os dados via MQTT
    String payload = "{";
    payload += "\"umidade\": ";
    payload += String(Humidity);
    payload += ", \"temperatura\": ";
    payload += String(temperature);
    payload += "}";

    payload.toCharArray(msg, 50);
    client.publish(mqtt_publish_topic, msg);
    Serial.print("Dados enviados: ");
    Serial.println(msg);
	// Aguarda 2 segundos antes da próxima leitura
	delay(2000);
  }

  
}
