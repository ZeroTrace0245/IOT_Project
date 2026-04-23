// TMC2209 Driver Diagnostics - Arduino Uno
// Use this to verify the driver is properly connected and responding

// TMC2209 Driver Control Pins
#define STEP_PIN 3
#define DIR_PIN 4
#define ENABLE_PIN 5
#define LIMIT_SWITCH_PIN 2
#define PRESSURE_SENSOR_PIN A0

// Test parameters
#define TEST_DELAY 1000  // milliseconds between tests

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println("\n\n========================================");
  Serial.println("TMC2209 DRIVER DIAGNOSTICS");
  Serial.println("========================================\n");

  // Configure pins
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(PRESSURE_SENSOR_PIN, INPUT);

  // Set initial states
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(ENABLE_PIN, HIGH);  // Motor disabled initially

  Serial.println("Pin Configuration: OK");
  Serial.println("STEP_PIN: Pin 3");
  Serial.println("DIR_PIN: Pin 4");
  Serial.println("ENABLE_PIN: Pin 5");
  Serial.println("LIMIT_SWITCH_PIN: Pin 2");
  Serial.println("PRESSURE_SENSOR_PIN: A0\n");
}

void loop() {
  Serial.println("========================================");
  Serial.println("TEST CYCLE");
  Serial.println("========================================\n");

  // Test 1: Motor Enable/Disable Detection
  testMotorEnable();
  delay(TEST_DELAY);

  // Test 2: Direction Signal
  testDirection();
  delay(TEST_DELAY);

  // Test 3: Step Signal
  testStepSignal();
  delay(TEST_DELAY);

  // Test 4: Limit Switch
  testLimitSwitch();
  delay(TEST_DELAY);

  // Test 5: Pressure Sensor
  testPressureSensor();
  delay(TEST_DELAY);

  // Test 6: Full Motor Movement
  testMotorMovement();
  delay(TEST_DELAY);

  Serial.println("\n\n");
  delay(3000);  // Wait before next cycle
}

// Test 1: Motor Enable/Disable
void testMotorEnable() {
  Serial.println("[TEST 1] Motor Enable/Disable Detection");
  Serial.println("-----------------------------------------");

  // Disable motor
  digitalWrite(ENABLE_PIN, HIGH);
  Serial.println("ENABLE_PIN set to HIGH (motor DISABLED)");
  delay(500);

  // Enable motor
  digitalWrite(ENABLE_PIN, LOW);
  Serial.println("ENABLE_PIN set to LOW (motor ENABLED)");

  // Check if motor locks (becomes hard to turn)
  Serial.println("✓ Check: Motor should be harder to turn now (if NEMA 17 is connected)");
  Serial.println("✓ Indicator: Green LED on driver should light up\n");

  delay(1000);
  digitalWrite(ENABLE_PIN, HIGH);  // Leave disabled
}

// Test 2: Direction Control
void testDirection() {
  Serial.println("[TEST 2] Direction Control");
  Serial.println("-----------------------------------------");

  // Enable motor for this test
  digitalWrite(ENABLE_PIN, LOW);
  delay(100);

  // Forward direction
  digitalWrite(DIR_PIN, HIGH);
  Serial.println("DIR_PIN set to HIGH (FORWARD direction)");
  delay(500);

  // Reverse direction
  digitalWrite(DIR_PIN, LOW);
  Serial.println("DIR_PIN set to LOW (REVERSE direction)");
  delay(500);

  Serial.println("✓ Check: Motor should be able to switch directions\n");

  digitalWrite(ENABLE_PIN, HIGH);  // Disable after test
}

// Test 3: Step Signal
void testStepSignal() {
  Serial.println("[TEST 3] Step Signal / Motor Movement");
  Serial.println("-----------------------------------------");

  // Enable motor
  digitalWrite(ENABLE_PIN, LOW);
  delay(100);

  // Set forward direction
  digitalWrite(DIR_PIN, HIGH);

  Serial.println("Sending 50 step pulses (forward)...");

  // Send 50 steps
  for (int i = 0; i < 50; i++) {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(2);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(500);
  }

  Serial.println("✓ 50 steps sent");
  Serial.println("✓ Check: Motor shaft should have rotated slightly");
  delay(500);

  // Reverse and send 50 steps back
  Serial.println("Sending 50 step pulses (reverse)...");
  digitalWrite(DIR_PIN, LOW);

  for (int i = 0; i < 50; i++) {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(2);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(500);
  }

  Serial.println("✓ 50 reverse steps sent");
  Serial.println("✓ Check: Motor should have returned to original position\n");

  digitalWrite(ENABLE_PIN, HIGH);  // Disable after test
}

// Test 4: Limit Switch
void testLimitSwitch() {
  Serial.println("[TEST 4] Limit Switch Detection");
  Serial.println("-----------------------------------------");

  int switchState = digitalRead(LIMIT_SWITCH_PIN);

  Serial.print("Limit Switch State: ");
  if (switchState == HIGH) {
	Serial.println("HIGH (NOT TRIGGERED - expected at rest)");
	Serial.println("✓ Check: Press the limit switch and it should read LOW");
  } else {
	Serial.println("LOW (TRIGGERED)");
	Serial.println("⚠ Check: Is the limit switch pressed? Release it.");
  }

  // Monitor for 2 seconds
  Serial.println("\nMonitoring for 2 seconds...");
  unsigned long startTime = millis();
  bool stateChanged = false;

  while (millis() - startTime < 2000) {
	int newState = digitalRead(LIMIT_SWITCH_PIN);
	if (newState != switchState) {
	  stateChanged = true;
	  Serial.print("✓ State changed to: ");
	  Serial.println(newState == HIGH ? "HIGH" : "LOW");
	  switchState = newState;
	}
  }

  if (!stateChanged) {
	Serial.println("⚠ No state change detected - check wiring");
  }
  Serial.println();
}

// Test 5: Pressure Sensor
void testPressureSensor() {
  Serial.println("[TEST 5] Pressure Sensor (A0)");
  Serial.println("-----------------------------------------");

  Serial.println("Reading pressure sensor 5 times...\n");

  for (int i = 0; i < 5; i++) {
	int pressureRaw = analogRead(PRESSURE_SENSOR_PIN);
	float voltage = (pressureRaw / 1023.0) * 5.0;

	Serial.print("Reading ");
	Serial.print(i + 1);
	Serial.print(": RAW=");
	Serial.print(pressureRaw);
	Serial.print(" (0-1023) | Voltage=");
	Serial.print(voltage, 2);
	Serial.println("V");

	delay(200);
  }

  Serial.println("\n✓ Pressure sensor readings:");
  Serial.println("  - 0 = 0V (no pressure)");
  Serial.println("  - 512 = ~2.5V (mid-range)");
  Serial.println("  - 1023 = ~5V (maximum pressure)\n");
}

// Test 6: Full Motor Movement Sequence
void testMotorMovement() {
  Serial.println("[TEST 6] Full Motor Movement Sequence");
  Serial.println("-----------------------------------------");

  Serial.println("Executing 200 steps (1 full revolution)...\n");

  // Enable motor
  digitalWrite(ENABLE_PIN, LOW);
  delay(100);

  // Forward
  Serial.println("→ FORWARD: 200 steps");
  digitalWrite(DIR_PIN, HIGH);

  for (int i = 0; i < 200; i++) {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(2);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(500);
  }

  Serial.println("✓ Completed\n");
  delay(1000);

  // Reverse back
  Serial.println("← REVERSE: 200 steps");
  digitalWrite(DIR_PIN, LOW);

  for (int i = 0; i < 200; i++) {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(2);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(500);
  }

  Serial.println("✓ Completed\n");

  // Disable motor
  digitalWrite(ENABLE_PIN, HIGH);

  Serial.println("Motor test complete!");
  Serial.println("✓ Check: Motor should have returned to starting position\n");
}
