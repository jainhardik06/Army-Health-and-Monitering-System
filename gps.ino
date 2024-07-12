#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Define the RX and TX pins for the NEO-6M GPS module
const int rxPin = 4;  // Connect RX to pin 4
const int txPin = 3;  // Connect TX to pin 3

// Create a SoftwareSerial object for communication with the NEO-6M GPS module
SoftwareSerial gpsSerial(rxPin, txPin);

// Create a TinyGPSPlus object
TinyGPSPlus gps;

void setup() {
  // Initialize serial communication with the computer
  Serial.begin(9600);

  // Initialize serial communication with the NEO-6M GPS module
  gpsSerial.begin(9600);

  Serial.println(F("GPS Module is initializing..."));
}

void loop() {
  // Check if data is available to read from the NEO-6M GPS module
  while (gpsSerial.available() > 0) {
    // Feed the GPS data into the TinyGPS++ library
    gps.encode(gpsSerial.read());
  }

  // Check if a valid GPS location is available
  if (gps.location.isUpdated()) {
    // Display the GPS location on the serial monitor
    Serial.print(F("Latitude: "));
    Serial.println(gps.location.lat(), 6);
    Serial.print(F("Longitude: "));
    Serial.println(gps.location.lng(), 6);
    Serial.print(F("Altitude (m): "));
    Serial.println(gps.altitude.meters());
    Serial.print(F("Speed (km/h): "));
    Serial.println(gps.speed.kmph());
    Serial.print(F("Satellites: "));
    Serial.println(gps.satellites.value());
    Serial.print(F("Date: "));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.println(gps.date.year());
    Serial.print(F("Time: "));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    Serial.println(gps.time.second());
    Serial.println();
  } else {
    Serial.println(F("Waiting for GPS signal..."));
  }

  // Wait for a short period before repeating the loop
  delay(1000);
}
