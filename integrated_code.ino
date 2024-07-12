#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Define the pin where the DHT11 data pin is connected
#define DHTPIN 5 // Change this pin according to your connection

// Define the type of sensor you're using, in this case, DHT11
#define DHTTYPE DHT11

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Define constants for other sensors and button
const int LDR_PIN = A0; // LDR module connected to A0 pin
const int HEART_RATE_PIN = A2; // Heart rate sensor connected to A2 pin
const int BUTTON_PIN = 7; // Button connected to digital pin 2

// Define constants for GPS module
const int GPS_RX_PIN = 4; // GPS TX to pin 4 (RX of SoftwareSerial)
const int GPS_TX_PIN = 3; // GPS RX to pin 3 (TX of SoftwareSerial)

// Initialize SoftwareSerial for GPS communication
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);

// Define LCD properties
const int LCD_ADDRESS = 0x27;
const int LCD_COLUMNS = 16;
const int LCD_ROWS = 2;
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS); // Initialize LCD display

void setup() {
  // Initialize I2C communication
  Wire.begin();

  // Initialize button pin as input with an internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize LCD display
  lcd.begin();
  lcd.backlight(); // Turn on backlight

  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the DHT sensor
  dht.begin();

  // Initialize GPS serial communication
  gpsSerial.begin(9600);

  Serial.println("DHT11 Humidity & Temperature Sensor\n\n");
  Serial.println("GPS Serial started.");
}

void loop() {
  // Read the state of the push button
  int buttonState = digitalRead(BUTTON_PIN);

  // Debugging: Print button state to Serial Monitor
  if (buttonState == LOW) {
    Serial.println("Button is pressed");
  } else {
    Serial.println("Button is not pressed");
  }

  // If the button is pressed, show "EMERGENCY" on LCD and Serial Monitor
  if (buttonState == LOW) { // Button is pressed
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EMERGENCY");
    Serial.println("EMERGENCY");
    delay(200); // Small delay to debounce the button press
  } else {
    // Proceed with other sensor readings and display updates
    // This part remains the same as your original code
    // Read LDR value
    int ldrValue = analogRead(LDR_PIN);

    // Determine brightness level
    String brightnessLevel;
    if (ldrValue < 100) {
      brightnessLevel = "Very bright";
    } else if (ldrValue < 200) {
      brightnessLevel = "Bright";
    } else if (ldrValue < 500) {
      brightnessLevel = "Dim";
    } else if (ldrValue < 800) {
      brightnessLevel = "Dim";
    } else {
      brightnessLevel = "Dark";
    }

    // Read humidity and temperature from the DHT11 sensor
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Read Failed!");
      delay(5000);
      return;
    }

    // Read heart rate sensor value
    int heartRate = analogRead(HEART_RATE_PIN) / 8.4; // Adjust the conversion based on your sensor

    // Display values on Serial Monitor
    Serial.print("LDR Value: ");
    Serial.println(ldrValue);
    Serial.print("Light: ");
    Serial.println(brightnessLevel);
    Serial.print("Temp (C): ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Heart Rate: ");
    Serial.println(heartRate);

    // Display values on LCD
    lcd.clear(); // Clear the LCD display
    lcd.setCursor(0, 0);
    lcd.print("LDR Value: ");
    lcd.print(ldrValue);
    delay(1000); // Wait 1 second

    lcd.setCursor(0, 1);
    lcd.print("Light: ");
    lcd.print(brightnessLevel);
    delay(1000); // Wait 1 second

    lcd.clear(); // Clear the LCD display
    lcd.setCursor(0, 0);
    lcd.print("Temp (C): ");
    lcd.print(temperature);
    delay(1000); // Wait 1 second

    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    delay(1000); // Wait 1 second

    lcd.clear(); // Clear the LCD display
    lcd.setCursor(0, 0);
    lcd.print("Heart Rate: ");
    lcd.print(heartRate);
    delay(1000); // Wait 1 second

    // Read GPS data
    if (gpsSerial.available() > 0) {
      String gpsData = "";
      while (gpsSerial.available() > 0) {
        char incomingByte = gpsSerial.read();
        gpsData += incomingByte;
      }

      // Print GPS data to Serial Monitor
      Serial.print("GPS Data: ");
      Serial.println(gpsData);

      // Display GPS data on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GPS Data:");
      lcd.setCursor(0, 1);
      lcd.print(gpsData.substring(0, 16)); // Display first 16 characters
      delay(2000); // Display for 2 seconds
    } else {
      Serial.println("No data available from GPS.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("");
      delay(1000); // Wait for 1 second before checking again
    }
  }
}
