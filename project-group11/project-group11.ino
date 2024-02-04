#include "DHT.h"

#define DHTPIN 5         // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11    // DHT 11

#define POWER_PIN 7      // Pin connected to the LED for the water sensor
#define SIGNAL_PIN A5    // Analog pin connected to the water sensor
#define SENSOR_MIN 0     // Minimum analog sensor reading
#define SENSOR_MAX 1023  // Maximum analog sensor reading

int value = 0;           // Variable to store the sensor value
int level = 0;           // Variable to store the water level

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(POWER_PIN, OUTPUT);   // Configure pin 7 as an OUTPUT for the water sensor LED
  digitalWrite(POWER_PIN, LOW); // Turn off the LED initially
  Serial.println(F("Urine Detector Activated"));
  
  dht.begin();
  pinMode(9, OUTPUT);  // Yellow LED light on pin 9 on the AVR board
  pinMode(10, OUTPUT); // Green LED light on pin 10 on the AVR board
  pinMode(11, OUTPUT); // Red LED light on pin 11 on the AVR board
}

void loop() {
  value = analogRead(SIGNAL_PIN); // Read the analog value from the water sensor
  level = map(value, SENSOR_MIN, SENSOR_MAX, 0, 100); // Map the sensor value to a percentage
  
  delay(2000); // Wait for 2 seconds before the next measurement
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  float h = dht.readHumidity();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h)) {
    Serial.println(F("Failed to read humidity from DHT sensor!"));
    return;
  }
  
  // Display humidity level
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));
  
  // Display water level
  Serial.print("Urine Content: ");
  Serial.print(level);
  Serial.println("%");

  // Determine LED status based on humidity level
  if (h <= 60 && level <= 10) {
    Serial.println("Your child has not urinated, they are asleep in bed. Okay!");
    digitalWrite(10, LOW); // Green LED OFF
    digitalWrite(9, HIGH); // Yellow LED ON
    digitalWrite(11, LOW); // Red LED OFF
    digitalWrite(POWER_PIN, LOW);   // Turn off the LED after reading
  }
  else if (h <= 90 &&  level <= 40) {
    Serial.println("Your child has started urinating in bed, wake them up to use the toilet!");
    digitalWrite(9, LOW);  // Yellow LED OFF
    digitalWrite(10, HIGH); // Green LED ON
    digitalWrite(11, LOW); // Red LED OFF
    digitalWrite(POWER_PIN, HIGH); // Turn on the water sensor LED
  }
  else {
    Serial.println("Your child has urinated, wake them up to change their clothes and beddings");
    digitalWrite(9, LOW);  // Yellow LED OFF
    digitalWrite(11, HIGH); // Red LED ON
    digitalWrite(10, LOW); // Green LED OFF
    digitalWrite(POWER_PIN, HIGH);   // Turn off the LED after reading
    delay(10);                      // wait 10 milliseconds
    digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF
    delay(10);
  }

}
