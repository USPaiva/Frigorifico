#include <DHT.h>

#define DHT_SENSOR_PIN 23       // Pino GPIO23 do ESP32 conectado ao DHT22
#define RELAY_FAN_PIN  18       // Pino GPIO18 do ESP32 conectado ao relé
#define DHT_SENSOR_TYPE DHT22

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

bool relayState = false; // Estado inicial do relé

void setup() {
  Serial.begin(9600);           // Inicializa a comunicação serial
  dht_sensor.begin();            // Inicializa o sensor DHT
  pinMode(RELAY_FAN_PIN, OUTPUT); // Define o pino do relé como saída
  Serial.println("Testando o sensor DHT22 e o relé...");
}

void loop() {
  // Leitura de temperatura e umidade
  float temperature = dht_sensor.readTemperature();
  float humidity = dht_sensor.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Falha ao ler o sensor DHT!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("Umidade: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  // Alterna o estado do relé
  relayState = !relayState;
  digitalWrite(RELAY_FAN_PIN, relayState ? HIGH : LOW);

  // Exibe o estado do relé no monitor serial
  if (relayState) {
    Serial.println("Relé ligado");
  } else {
    Serial.println("Relé desligado");
  }

  // Aguarda 2 segundos antes de alternar novamente
  delay(2000);
}
