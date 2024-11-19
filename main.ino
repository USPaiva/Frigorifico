#include <DHT.h>

#define DHT_SENSOR_PIN 23 // ESP32 pin GPIO23 connected to DHT22
#define RELAY_1_PIN    18 // ESP32 pin GPIO18 connected to relay 1
#define RELAY_2_PIN    19 // ESP32 pin GPIO19 connected to relay 2

#define DHT_SENSOR_TYPE DHT22

#define TEMP_UPPER_THRESHOLD  30 // Upper temperature threshold
#define TEMP_LOWER_THRESHOLD  15 // Lower temperature threshold

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup() {
  Serial.begin(9600); // Initialize serial
  dht_sensor.begin(); // Initialize the DHT sensor

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
  Serial.println("\nLendo a temperatura...");
  float temperature = dht_sensor.readTemperature();  // Read temperature in Celsius

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
  }

  // Aguarda 2 segundos antes da próxima leitura
  delay(2000);
}