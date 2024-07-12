// Pin for the push button
const int buttonPin = 2; // Change this pin as per your connection

// Variable to store the button state
int buttonState = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set the push button pin as input
  pinMode(buttonPin, INPUT);
}

void loop() {
  // Read the state of the push button
  buttonState = digitalRead(buttonPin);

  // Check if the button is pressed
  if (buttonState == HIGH) {
    // Button is pressed, send emergency signal
    Serial.println("Emergency Signal Activated!");
    // Add any additional actions here, such as turning on an alarm or activating a relay
    // Add a delay to prevent multiple detections due to button bouncing
    delay(1000);
  }
}
