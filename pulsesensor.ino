int pulsePin = A2;                 // Pulse Sensor purple wire connected to analog pin A2
int blinkPin = 13;                 // Pin to blink LED at each beat

volatile int BPM;                  // Variable to hold heart rate
volatile int Signal;               // Holds the incoming raw data
volatile int IBI = 600;            // Time interval between beats, in milliseconds
volatile boolean Pulse = false;    // True when user's live heartbeat is detected
volatile boolean QS = false;       // True when Arduino finds a beat

// Array to hold last ten IBI values
volatile int rate[10];

volatile unsigned long sampleCounter = 0;   // Used to determine pulse timing
volatile unsigned long lastBeatTime = 0;    // Used to find IBI
volatile int P = 512;                       // Used to find peak in pulse wave, seeded
volatile int T = 512;                       // Used to find trough in pulse wave, seeded
volatile int thresh = 550;                  // Used to find instant moment of heart beat, seeded
volatile int amp = 100;                     // Used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;          // Used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;        // Used to seed rate array so we startup with reasonable BPM

void setup() {
  pinMode(blinkPin, OUTPUT);         // Pin that will blink to heartbeat
  Serial.begin(115200);              // Initialize serial communication
  interruptSetup();                  // Set up to read Pulse Sensor signal every 2ms
}

void loop() {
  if (QS == true) {                   // If a heartbeat was found
    QS = false;                       // Reset the Quantified Self flag for next time
    // Output heartbeat data
    Serial.print("Heart-Beat Found, BPM: ");
    Serial.println(BPM);
  }
  delay(20);                          // Delay to prevent excessive looping
}

void interruptSetup() {
  TCCR2A = 0x02;                      // Disable PWM on digital pins 3 and 11, and go into CTC mode
  TCCR2B = 0x06;                      // Don't force compare, 256 prescaler
  OCR2A = 0X7C;                       // Set the top of the count to 124 for 500Hz sample rate
  TIMSK2 = 0x02;                      // Enable interrupt on match between Timer2 and OCR2A
  sei();                              // Enable global interrupts
}

ISR(TIMER2_COMPA_vect) {              // Triggered when Timer2 counts to 124
  cli();                              // Disable interrupts while we do this
  Signal = analogRead(pulsePin);      // Read the Pulse Sensor
  sampleCounter += 2;                 // Keep track of the time in ms

  int N = sampleCounter - lastBeatTime;  // Monitor the time since the last beat to avoid noise

  // Find the peak and trough of the pulse wave
  if (Signal < thresh && N > (IBI / 5) * 3) {
    if (Signal < T) {
      T = Signal;                   // Keep track of lowest point in pulse wave
    }
  }

  if (Signal > thresh && Signal > P) {
    P = Signal;                     // Keep track of highest point in pulse wave
  }

  // Look for the heart beat
  if (N > 250) {                    // Avoid high frequency noise
    if ((Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3)) {
      Pulse = true;                 // Set the Pulse flag when we think there is a pulse
      digitalWrite(blinkPin, HIGH); // Turn on LED
      IBI = sampleCounter - lastBeatTime;  // Measure time between beats in ms
      lastBeatTime = sampleCounter;        // Keep track of time for next pulse

      if (secondBeat) {             // If this is the second beat
        secondBeat = false;         // Clear secondBeat flag
        for (int i = 0; i <= 9; i++) {
          rate[i] = IBI;            // Seed the running total to get a realistic BPM at startup
        }
      }

      if (firstBeat) {              // If it's the first time we found a beat
        firstBeat = false;          // Clear firstBeat flag
        secondBeat = true;          // Set the second beat flag
        sei();                      // Enable interrupts again
        return;                     // IBI value is unreliable so discard it
      }

      // Keep a running total of the last 10 IBI values
      word runningTotal = 0;        // Clear the runningTotal variable

      for (int i = 0; i <= 8; i++) {
        rate[i] = rate[i + 1];      // Shift data in the rate array and drop the oldest IBI value
        runningTotal += rate[i];    // Add up the 9 oldest IBI values
      }

      rate[9] = IBI;                // Add the latest IBI to the rate array
      runningTotal += rate[9];      // Add the latest IBI to runningTotal
      runningTotal /= 10;           // Average the last 10 IBI values
      BPM = 60000 / runningTotal;   // Calculate BPM
      QS = true;                    // Set Quantified Self flag
    }
  }

  if (Signal < thresh && Pulse == true) {  // When the values are going down, the beat is over
    digitalWrite(blinkPin, LOW);           // Turn off LED
    Pulse = false;                          // Reset Pulse flag
    amp = P - T;                            // Get amplitude of the pulse wave
    thresh = amp / 2 + T;                   // Set threshold at 50% of the amplitude
    P = thresh;                             // Reset variables for next time
    T = thresh;
  }

  if (N > 2500) {                           // If 2.5 seconds go by without a beat
    thresh = 550;                           // Set threshold default
    P = 512;                                // Reset P default
    T = 512;                                // Reset T default
    lastBeatTime = sampleCounter;           // Update lastBeatTime
    firstBeat = true;                       // Reset flags to avoid noise
    secondBeat = false;
  }

  sei();                                    // Enable interrupts when done
}
