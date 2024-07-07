#include <DHT.h>
#include <LiquidCrystal.h>

// Constants for DHT sensor
#define DHTPIN 2  // Pin where the DHT sensor is connected
#define DHTTYPE DHT22  // Type of DHT sensor (DHT11 or DHT22)

// Constants for soil moisture sensor
const int soilMoisturePin = A0;  // Analog pin for soil moisture sensor

// Constants for relay modules controlling pump and lamp
const int pumpRelayPin = 3;  // Digital pin for pump relay
const int lampRelayPin = 4;  // Digital pin for lamp relay

// Constants for LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Constants for set constraints
const int idealTempMin = 67;
const int idealTempMax = 73;
const int lampOnHoursIdeal = 12;
const int lampOnHoursLow = 14;
const int lampOnHoursHigh = 10;
const int idealMoisture = 700;  // Example threshold value for soil moisture

// Variables to store sensor readings
float temperature;
float humidity;
int soilMoisture;

// Function prototypes
void readSensors();
void controlActuators();

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD
  lcd.begin(16, 2);

  // Initialize relay pins as outputs
  pinMode(pumpRelayPin, OUTPUT);
  pinMode(lampRelayPin, OUTPUT);
}

void loop() {
  // Read sensor data
  readSensors();

  // Control actuators based on sensor data
  controlActuators();

  // Display sensor readings on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Moisture: ");
  lcd.print(soilMoisture);
  lcd.print(" ");

  delay(1000);  // Delay to avoid flooding the LCD with updates
}

void readSensors() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();
  temperature = dht.readTemperature(); // Read temperature in Celsius

  // Read soil moisture sensor
  soilMoisture = analogRead(soilMoisturePin);
}

void controlActuators() {
  // Control lamp based on temperature
  if (temperature >= idealTempMin && temperature <= idealTempMax) {
    // Ideal temperature range
    digitalWrite(lampRelayPin, HIGH);  // Turn on lamp
    delay(lampOnHoursIdeal * 3600000); // Convert hours to milliseconds
    digitalWrite(lampRelayPin, LOW);   // Turn off lamp
  } else if (temperature < idealTempMin) {
    // Temperature too low
    digitalWrite(lampRelayPin, HIGH);  // Turn on lamp
    delay(lampOnHoursLow * 3600000);   // Convert hours to milliseconds
    digitalWrite(lampRelayPin, LOW);   // Turn off lamp
  } else {
    // Temperature too high
    digitalWrite(lampRelayPin, HIGH);  // Turn on lamp
    delay(lampOnHoursHigh * 3600000);  // Convert hours to milliseconds
    digitalWrite(lampRelayPin, LOW);   // Turn off lamp
  }

  // Control pump based on soil moisture
  if (soilMoisture < idealMoisture) {
    digitalWrite(pumpRelayPin, HIGH); // Turn on pump
    delay(5000);  // Example: water for 5 seconds
    digitalWrite(pumpRelayPin, LOW);  // Turn off pump
  } else {
    digitalWrite(pumpRelayPin, LOW);  // Ensure pump is off
  }
}
