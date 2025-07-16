const int numSensors = 2; // Number of sensors
const int sensorPins[] = {2, 4}; // SIG pins of the ultrasonic sensors
const int ledPins[] = {10, 11}; // LED pins
const int maxDistance = 200; // Max distance in cm to detect vehicles
const int ledOffDelay = 500; // Delay in milliseconds after vehicle leaves

unsigned long errorStartTime = 0; // Timer for error condition
bool errorCondition = false; // Tracks if an error is being monitored
bool vehicleInRange[] = {false, false}; // Flags to track if a vehicle is in range
int vehicleCounts[] = {0, 0}; // Vehicle counts for each sensor

long getDistance(int sensorPin) {
  pinMode(sensorPin, OUTPUT);   // Set SIG pin as OUTPUT to send trigger signal
  digitalWrite(sensorPin, LOW);
  delayMicroseconds(2);         // Ensure a clean LOW signal
  digitalWrite(sensorPin, HIGH);
  delayMicroseconds(10);        // Send 10 microsecond HIGH pulse
  digitalWrite(sensorPin, LOW);

  pinMode(sensorPin, INPUT);    // Set SIG pin as INPUT to read echo
  long duration = pulseIn(sensorPin, HIGH, 20000); // Read echo time (timeout 20ms)
  long distance = duration * 0.034 / 2;  // Convert to distance in cm
  return distance;
}

void setup() {
  for (int i = 0; i < numSensors; i++) {
    pinMode(ledPins[i], OUTPUT); // Set LED pins as OUTPUT
  }
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    long distance = getDistance(sensorPins[i]);
    if (distance > 0 && distance <= maxDistance) {
      if (!vehicleInRange[i]) {
        vehicleInRange[i] = true; // Set the flag indicating a vehicle is in range
        vehicleCounts[i]++; // Increment vehicle count only when entering range
        Serial.print("Sensor ");
        Serial.print(i + 1);
        Serial.print(": Vehicle detected. Total count: ");
        Serial.println(vehicleCounts[i]);
      }
      digitalWrite(ledPins[i], HIGH); // Keep LED on while vehicle is in range
    } else {
      if (vehicleInRange[i]) {
        delay(ledOffDelay); // Add delay before turning off the LED
      }
      vehicleInRange[i] = false; // Reset the flag when vehicle leaves range
      digitalWrite(ledPins[i], LOW); // Turn off LED when vehicle is out of range
    }
  }

  // Check if the vehicle counts are not equal
  if (vehicleCounts[0] != vehicleCounts[1]) {
    if (!errorCondition) {
      errorStartTime = millis(); // Start timing the error condition
      errorCondition = true; // Begin monitoring the error
    } else if (millis() - errorStartTime > 30000) { // 30 seconds
      Serial.println("Error: Vehicle counts at sensors are not equal!");
      delay(30000);
    }
  } else {
    errorCondition = false; // Reset the error condition
  }
}
