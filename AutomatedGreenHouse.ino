#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Constants for DHT sensor (DHT11)
#define DHTPIN 7     // Pin where the DHT sensor is connected
#define DHTTYPE DHT11 // Type of DHT sensor (DHT11 or DHT22)

// Constants for soil moisture sensor
const int soilMoisturePin = A0; // Analog pin for soil moisture sensor

// Constants for relay modules controlling pump and lamp
const int pumpRelayPin = 2; // Digital pin for pump relay
const int lampRelayPin = 3; // Digital pin for lamp relay

// Constants for LCD (using 2004 LCD with I2C)
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 columns and 4 rows

// Constants for set constraints
const int idealTempMin = 67;
const int idealTempMax = 73;
const int lampOnHoursIdeal = 12;
const int lampOnHoursLow = 14;
const int lampOnHoursHigh = 10;
const int idealMoisture = 700; // Example threshold value for soil moisture
const int waterTimeSeconds = 5; // in seconds

// Variables to store sensor readings
float temperature;
float humidity;
int soilMoisture;
unsigned long lastMoistureReadTime = 0;
const unsigned long moistureReadInterval = 86400000; // Interval for moisture read (1 day in milliseconds)
unsigned long lastTempHumReadTime = 0;
const unsigned long tempHumReadInverval = 60000; // Interval for moisture read (1 minute in milliseconds)

// Function prototypes
void readSensors();
void controlActuators();

DHT dht11(DHTPIN, DHTTYPE);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize LCD via I2C
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Ervas!");

  // Initialize relay pins as outputs
  pinMode(pumpRelayPin, OUTPUT);
  pinMode(lampRelayPin, OUTPUT);

  // Initialize DHT sensor
  dht11.begin();
  readTempHumSensor(); //trigger temperature and humidity at init
  readMoistureSensor(); //trigger moisture check at init
}

void loop() {
  // Read sensor data periodically
  unsigned long currentMillis = millis();
  if (currentMillis - lastMoistureReadTime >= moistureReadInterval) {
    readMoistureSensor();  // Read sensors if interval has elapsed
    lastMoistureReadTime = currentMillis;  // Update last read time
  }

  if (currentMillis - lastTempHumReadTime >= tempHumReadInverval) {
    readTempHumSensor(); // Read temp and hum sensor if interval has elapsed
    lastTempHumReadTime = currentMillis;
  }

  // Display sensor readings on LCD
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print((char)223);  // degree symbol
  lcd.print("F");

  lcd.setCursor(0, 2);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 3);
  lcd.print("Moisture: ");
  lcd.print(soilMoisture);
  lcd.print("%");

  // Control actuators based on sensor data
  controlActuators();

  delay(1000); // Delay to avoid flooding the LCD with updates
}

void readTempHumSensor() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  humidity = dht11.readHumidity();
  temperature = dht11.readTemperature(); // Read temperature in Celsius
  // Convert temperature to Fahrenheit
  temperature = temperature * 1.8 + 32;
}

void readMoistureSensor() {
  // Read soil moisture sensor
  soilMoisture = analogRead(soilMoisturePin);
  soilMoisture = (1023 - soilMoisture) / 10.23;
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
    delay(waterTimeSeconds * 1000);  // in ms
    digitalWrite(pumpRelayPin, LOW); // Turn off pump
  } else {
    digitalWrite(pumpRelayPin, LOW); // Ensure pump is off
  }
}
