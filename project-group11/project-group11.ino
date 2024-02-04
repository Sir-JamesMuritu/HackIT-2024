#include "DHT.h"

#define DHTPIN 5     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

#define POWER_PIN  7
#define SIGNAL_PIN A5
#define SENSOR_MIN 0
#define SENSOR_MAX 521

int value = 0; // variable to store the sensor value
int level = 0; // variable to store the water level

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(POWER_PIN, OUTPUT);   // configure D7 pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
  Serial.println(F("Urine Detector Activated"));
  
  dht.begin();
  pinMode(9, OUTPUT);  // Yellow LED light on pin 9 on the AVR board
  pinMode(10, OUTPUT);  // Green LED light on pin 10 on the AVR board
  pinMode(11, OUTPUT);  // Red LED light on pin 11 on the AVR board
}

void loop() {
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  level = map(value, SENSOR_MIN, SENSOR_MAX, 0, 4); // 4 levels
  
  delay(2000);  // Wait 2 seconds between measurements
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  float h = dht.readHumidity();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h)) {
    Serial.println(F("Failed to read humidity from DHT sensor!"));
    return;
  }

  if (h <= 60) {
    Serial.println("Your child has not urinated, they are asleep in bed. Okay!");
    digitalWrite(10, LOW);  // Green LED OFF
    digitalWrite(9, HIGH);  // Yellow LED ON
    digitalWrite(11, LOW);  // Red LED OFF
  }
  else if (h <= 90) {
    Serial.println("Your child has started urinating in bed, wake them up to use the toilet!");
    digitalWrite(9, LOW);   // Yellow LED OFF
    digitalWrite(10, HIGH); // Green LED ON
    digitalWrite(11, LOW);  // Red LED OFF
  }
  else {
    Serial.println("Your child has urinated, wake them up to change their clothes and beddings");
    digitalWrite(9, LOW);   // Yellow LED OFF
    digitalWrite(11, HIGH); // Red LED ON
    digitalWrite(10, LOW);  // Green LED OFF
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));

  Serial.print("Water level: ");
  Serial.println(level);
  delay(1000);  // Wait for 1 second before next loop iteration
}
