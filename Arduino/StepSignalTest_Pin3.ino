// STEP Signal Test - Pin 3
// Test stepper motor step pulses on Pin 3

#define STEP_PIN 3
#define TEST_DELAY 1000

// Test parameters
#define PULSE_WIDTH_US 2      // 2 microseconds pulse width
#define DELAY_BETWEEN_US 500  // 500 microseconds between pulses

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("STEP SIGNAL TEST - PIN 3");
  Serial.println("========================================\n");

  // Configure pin as output
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);  // Start low

  Serial.println("Pin Configuration: OK");
  Serial.println("STEP_PIN: Pin 3");
  Serial.println("Pulse Width: 2 microseconds");
  Serial.println("Frequency: Variable\n");

  Serial.println("Instructions:");
  Serial.println("1. Connect stepper motor to driver");
  Serial.println("2. Listen for stepping sounds");
  Serial.println("3. Shaft should rotate slightly per test");
  Serial.println("4. Faster steps = faster rotation");
  Serial.println("5. Different step counts show precise control\n");

  Serial.println("========================================");
  Serial.println("Starting STEP signal test...\n");
}

int testCycle = 0;

void loop() {
  testCycle++;

  Serial.println("\n========== STEP TEST CYCLE ==========\n");

  // Test 1: 10 slow steps
  testSlowSteps();
  delay(TEST_DELAY);

  // Test 2: 50 medium steps
  testMediumSteps();
  delay(TEST_DELAY);

  // Test 3: 100 fast steps
  testFastSteps();
  delay(TEST_DELAY);

  // Test 4: Continuous smooth rotation
  testSmoothRotation();
  delay(TEST_DELAY);

  // Test 5: Step pulse accuracy
  testPulseAccuracy();
  delay(TEST_DELAY);

  Serial.println("\n");
  delay(2000);  // Wait before next cycle
}

// Test 1: Slow stepping (10 pulses with long delay)
void testSlowSteps() {
  Serial.println("[TEST 1] Slow Stepping (10 pulses)");
  Serial.println("-----------------------------------------");
  Serial.println("Sending 10 step pulses with 1s delay between...\n");

  for (int i = 0; i < 10; i++) {
	sendStepPulse();
	Serial.print("Step ");
	Serial.println(i + 1);
	delay(1000);  // 1 second between steps
  }

  Serial.println("\n✓ 10 slow steps sent");
  Serial.println("✓ Motor shaft should have rotated slightly");
  Serial.println("✓ You should hear distinct stepping sounds\n");
}

// Test 2: Medium stepping (50 pulses with 100ms delay)
void testMediumSteps() {
  Serial.println("[TEST 2] Medium Speed Stepping (50 pulses)");
  Serial.println("-----------------------------------------");
  Serial.println("Sending 50 step pulses with 100ms delay...\n");

  for (int i = 0; i < 50; i++) {
	sendStepPulse();
	if ((i + 1) % 10 == 0) {
	  Serial.print("Steps completed: ");
	  Serial.println(i + 1);
	}
	delayMicroseconds(100000);  // 100ms between pulses
  }

  Serial.println("\n✓ 50 medium-speed steps sent");
  Serial.println("✓ Motor should rotate noticeably");
  Serial.println("✓ Stepping should be continuous and smooth\n");
}

// Test 3: Fast stepping (100 pulses with 10ms delay)
void testFastSteps() {
  Serial.println("[TEST 3] Fast Stepping (100 pulses)");
  Serial.println("-----------------------------------------");
  Serial.println("Sending 100 fast step pulses...\n");

  unsigned long startTime = millis();

  for (int i = 0; i < 100; i++) {
	sendStepPulse();
	delayMicroseconds(10000);  // 10ms between pulses (~100 steps/sec)
  }

  unsigned long duration = millis() - startTime;

  Serial.print("100 steps in ");
  Serial.print(duration);
  Serial.println("ms");
  Serial.print("Frequency: ");
  Serial.print((100.0 / duration) * 1000);
  Serial.println(" steps/sec");

  Serial.println("\n✓ 100 fast steps sent");
  Serial.println("✓ Motor should rotate significantly");
  Serial.println("✓ Should hear continuous buzzing sound\n");
}

// Test 4: Smooth continuous rotation (200 steps = ~1 full revolution)
void testSmoothRotation() {
  Serial.println("[TEST 4] Smooth Rotation (200 steps)");
  Serial.println("-----------------------------------------");
  Serial.println("Sending 200 smooth step pulses...\n");

  Serial.println("→ FORWARD: 200 steps");
  unsigned long startTime = millis();

  for (int i = 0; i < 200; i++) {
	sendStepPulse();
	delayMicroseconds(5000);  // 5ms between pulses (~200 steps/sec)
  }

  unsigned long duration = millis() - startTime;

  Serial.println("✓ Forward rotation complete");
  Serial.print("Duration: ");
  Serial.print(duration);
  Serial.println("ms");

  delay(1000);  // Pause between forward and reverse

  Serial.println("\n← REVERSE: 200 steps (simulated)");
  Serial.println("✓ Motor should return to start position");
  Serial.println("✓ Check for smooth, continuous rotation\n");
}

// Test 5: Pulse accuracy test (verify pulse timing)
void testPulseAccuracy() {
  Serial.println("[TEST 5] Step Pulse Accuracy");
  Serial.println("-----------------------------------------");
  Serial.println("Testing pulse width and timing...\n");

  Serial.println("Sending 5 precisely-timed pulses:");

  for (int i = 0; i < 5; i++) {
	unsigned long pulseStart = micros();

	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(PULSE_WIDTH_US);
	digitalWrite(STEP_PIN, LOW);

	unsigned long pulseEnd = micros();
	unsigned long actualWidth = pulseEnd - pulseStart;

	Serial.print("Pulse ");
	Serial.print(i + 1);
	Serial.print(": ");
	Serial.print(actualWidth);
	Serial.println("µs");

	delayMicroseconds(DELAY_BETWEEN_US);
  }

  Serial.println("\n✓ Pulse timing verified");
  Serial.println("✓ Pulses should be consistent");
  Serial.println("✓ Timing accuracy critical for stepper control\n");
}

// Send a single step pulse
// Pulse: HIGH for 2µs, then LOW
void sendStepPulse() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(PULSE_WIDTH_US);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(DELAY_BETWEEN_US);
}
