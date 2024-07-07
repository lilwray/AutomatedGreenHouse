#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Constants for DHT sensor (DHT11)
#define DHTPIN 7     // Pin where the DHT sensor is connected
#define DHTTYPE DHT11 // Type of DHT sensor (DHT11 or DHT22)

// Constants for soil moisture sensor
const int soilMoisturePin = A0; // Analog pin for soil moisture sensor

// Constants for relay modules controlling pump and lamp
const int pumpRelayPin = 2; // Digital pin for pump relay
const int lampRelayPin = 3; // Digital pin for lamp relay

// Constants for LCD (using 2004 LCD)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // RS, EN, D4, D5, D6, D7

// Constants for set constraints
const int idealTempMin = 67;
const int idealTempMax = 73;
const int lampOnHoursIdeal = 12;
const int lampOnHoursLow = 14;
const int lampOnHoursHigh = 10;
const int idealMoisture = 700; // Example threshold value for soil moisture
const int waterTimeSeconds = 5; //in seconds

// Variables to store sensor readings
float temperature;
float humidity;
int soilMoisture;
unsigned long lastMoistureReadTime = 0;
const unsigned long moistureReadInterval = 86400000; // Interval for moisture read (1 day in milliseconds)

// Function prototypes
void readSensors();
void controlActuators();

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD with the number of columns and rows
  lcd.begin(20, 4);

  // Initialize relay pins as outputs
  pinMode(pumpRelayPin, OUTPUT);
  pinMode(lampRelayPin, OUTPUT);
}

void loop() {
  // Read sensor data periodically
  unsigned long currentMillis = millis();
  if (currentMillis - lastMoistureReadTime >= moistureReadInterval) {
    readSensors();  // Read sensors if interval has elapsed
    lastMoistureReadTime = currentMillis;  // Update last read time
  }

  // Control actuators based on sensor data
  controlActuators();

  // Display sensor readings on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");

  lcd.setCursor(0, 2);
  lcd.print("Moisture: ");
  lcd.print(soilMoisture);
  lcd.print(" ");

  delay(1000); // Delay to avoid flooding the LCD with updates
}

void readSensors() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  humidity = dht.readHumidity();
  temperature = dht.readTemperature(); // Read temperature in Celsius

  // Read soil moisture sensor
  soilMoisture = analogRead(soilMoisturePin);
}

void controlActuators() {
  // Control lamp based on temperature
  if (temperature >= idealTempMin && temperature <= idealTempMax) {
    // Ideal temperature range
    digitalWrite(lampRelayPin, HIGH); // Turn on lamp
    delay(lampOnHoursIdeal * 3600000); // Convert hours to milliseconds
    digitalWrite(lampRelayPin, LOW);  // Turn off lamp
  } else if (temperature < idealTempMin) {
    // Temperature too low
    digitalWrite(lampRelayPin, HIGH); // Turn on lamp
    delay(lampOnHoursLow * 3600000);  // Convert hours to milliseconds
    digitalWrite(lampRelayPin, LOW);  // Turn off lamp
  } else {
    // Temperature too high
    digitalWrite(lampRelayPin, HIGH); // Turn on lamp
    delay(lampOnHoursHigh * 3600000); // Convert hours to milliseconds
    digitalWrite(lampRelayPin, LOW);  // Turn off lamp
  }

  // Control pump based on soil moisture
  if (soilMoisture < idealMoisture) {
    digitalWrite(pumpRelayPin, HIGH); // Turn on pump
    delay(waterTimeSeconds * 1000);  // in milliseconds (ms)
    digitalWrite(pumpRelayPin, LOW); // Turn off pump
  } else {
    digitalWrite(pumpRelayPin, LOW); // Ensure pump is off
  }
}
