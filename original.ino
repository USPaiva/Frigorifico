/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-cooling-system-using-dht-sensor
 */

#include <DHT.h>

#define DHT_SENSOR_PIN 23 // ESP32 pin GPIO23 connected to DHT22
#define RELAY_FAN_PIN  18 // ESP32 pin GPIO18 connected to relay

#define DHT_SENSOR_TYPE DHT22

#define TEMP_UPPER_THRESHOLD  30 // upper temperature threshold
#define TEMP_LOWER_THRESHOLD  15 // lower temperature threshold

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup() {
  Serial.begin(9600); // initialize serial
  dht_sensor.begin(); // initialize the DHT sensor
}

void loop() {
  float temperature = dht_sensor.readTemperature();;  // read temperature in Celsius

  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    if (temperature > TEMP_UPPER_THRESHOLD) {
      Serial.println("Turn the fan on");
      digitalWrite(RELAY_FAN_PIN, HIGH); // turn on
    } else if (temperature < TEMP_LOWER_THRESHOLD) {
      Serial.println("Turn the fan off");
      digitalWrite(RELAY_FAN_PIN, LOW); // turn off
    }
  }

  // wait a 2 seconds between readings
  delay(2000);
}
