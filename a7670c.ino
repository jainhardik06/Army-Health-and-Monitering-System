#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// LCD module connections (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Define software serial pins for GPS
SoftwareSerial gpsSerial(10, 9); // RX, TX

unsigned long lastConnectionAttempt = 0;
const unsigned long connectionInterval = 1800000; // 30 minutes in milliseconds

void setup() {
  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  // Begin serial communication with GPS module and Serial Monitor
  Serial.begin(115200);
  gpsSerial.begin(9600);

  // Print debug message
  Serial.println("Setup complete.");
}

void loop() {
  // Read GPS data
  String gpsData = getGpsData();
  if (gpsData.length() > 0) {
    Serial.println("GPS Data Received:");
    Serial.println(gpsData);

    // Check if the GPS data contains GPGGA string which has location data
    if (gpsData.startsWith("$GPGGA")) {
      String latitude = parseLatitude(gpsData);
      String longitude = parseLongitude(gpsData);

      // Display latitude and longitude on LCD
      lcd.clear();
      lcd.print("Lat: ");
      lcd.print(latitude);
      lcd.setCursor(0, 1);
      lcd.print("Long: ");
      lcd.print(longitude);

      // Print latitude and longitude to Serial Monitor
      Serial.println("Latitude: " + latitude);
      Serial.println("Longitude: " + longitude);
    }
  }

  // Attempt to connect with satellites every 30 minutes
  unsigned long currentMillis = millis();
  if (currentMillis - lastConnectionAttempt >= connectionInterval) {
    lastConnectionAttempt = currentMillis;
    Serial.println("Attempting to connect with satellites...");
    gpsSerial.println("AT+CGNSPWR=1"); // Power on GPS
    delay(1000);
    gpsSerial.println("AT+CGNSSEQ=R"); // Restart GPS
  }
}

// Function to get GPS data
String getGpsData() {
  gpsSerial.println("AT+CGNSINF");
  delay(1000);
  String gpsData = "";
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    gpsData += c;
  }
  return gpsData;
}

// Function to parse latitude from GPGGA sentence
String parseLatitude(String gpsData) {
  int startIndex = gpsData.indexOf(',') + 1;
  startIndex = gpsData.indexOf(',', startIndex) + 1;
  int endIndex = gpsData.indexOf(',', startIndex);
  return gpsData.substring(startIndex, endIndex);
}

// Function to parse longitude from GPGGA sentence
String parseLongitude(String gpsData) {
  int startIndex = gpsData.indexOf(',', gpsData.indexOf(',') + 1) + 1;
  startIndex = gpsData.indexOf(',', startIndex) + 1;
  startIndex = gpsData.indexOf(',', startIndex) + 1;
  int endIndex = gpsData.indexOf(',', startIndex);
  return gpsData.substring(startIndex, endIndex);
}
