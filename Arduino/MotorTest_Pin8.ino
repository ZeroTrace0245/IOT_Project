// Motor Test - Pin 8
// Test PWM motor control on Pin 8

#define MOTOR_PIN 8

// PWM parameters
#define MIN_SPEED 0
#define MAX_SPEED 255
#define TEST_DELAY 1000

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("MOTOR TEST - PIN 8 (PWM)");
  Serial.println("========================================\n");

  // Configure pin as output
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);  // Start stopped

  Serial.println("Pin Configuration: OK");
  Serial.println("MOTOR_PIN: Pin 8 (PWM capable)");
  Serial.println("Speed Range: 0-255 (0% - 100%)\n");

  Serial.println("Instructions:");
  Serial.println("1. Listen for motor sound changes");
  Serial.println("2. Motor should start quiet, get louder");
  Serial.println("3. Feel for vibration increasing");
  Serial.println("4. Maximum speed = loudest, fastest spin");
  Serial.println("5. Then decreases back to stop\n");

  Serial.println("========================================");
  Serial.println("Starting motor test...\n");
}

int previousSpeed = -1;

void loop() {
  Serial.println("\n========== MOTOR TEST CYCLE ==========\n");

  // Test 1: Speed ramp up (0 to 255)
  testSpeedRampUp();
  delay(TEST_DELAY);

  // Test 2: Speed ramp down (255 to 0)
  testSpeedRampDown();
  delay(TEST_DELAY);

  // Test 3: Fixed speeds
  testFixedSpeeds();
  delay(TEST_DELAY);

  // Test 4: Pulse test
  testMotorPulse();
  delay(TEST_DELAY);

  Serial.println("\n");
  delay(2000);  // Wait before next cycle
}

// Test 1: Gradually increase speed from 0 to 255
void testSpeedRampUp() {
  Serial.println("[TEST 1] Speed Ramp UP (0% → 100%)");
  Serial.println("-----------------------------------------");
  Serial.println("Increasing motor speed gradually...\n");

  for (int speed = 0; speed <= 255; speed += 25) {
	analogWrite(MOTOR_PIN, speed);
	int percentage = (speed * 100) / 255;
	Serial.print("Speed: ");
	Serial.print(speed);
	Serial.print(" (");
	Serial.print(percentage);
	Serial.println("%)");
	delay(300);
  }

  Serial.println("\n✓ Motor should be at maximum speed");
  Serial.println("✓ Should hear/feel vibration at full power\n");
}

// Test 2: Gradually decrease speed from 255 to 0
void testSpeedRampDown() {
  Serial.println("[TEST 2] Speed Ramp DOWN (100% → 0%)");
  Serial.println("-----------------------------------------");
  Serial.println("Decreasing motor speed gradually...\n");

  for (int speed = 255; speed >= 0; speed -= 25) {
	analogWrite(MOTOR_PIN, speed);
	int percentage = (speed * 100) / 255;
	Serial.print("Speed: ");
	Serial.print(speed);
	Serial.print(" (");
	Serial.print(percentage);
	Serial.println("%)");
	delay(300);
  }

  Serial.println("\n✓ Motor should slow down gradually");
  Serial.println("✓ Should stop completely at speed 0\n");
}

// Test 3: Test specific fixed speeds
void testFixedSpeeds() {
  Serial.println("[TEST 3] Fixed Speed Tests");
  Serial.println("-----------------------------------------\n");

  int speeds[] = {50, 100, 150, 200, 255};

  for (int i = 0; i < 5; i++) {
	int speed = speeds[i];
	analogWrite(MOTOR_PIN, speed);
	int percentage = (speed * 100) / 255;

	Serial.print("Testing Speed ");
	Serial.print(speed);
	Serial.print(" (");
	Serial.print(percentage);
	Serial.println("%):");
	Serial.println("  Listen for consistent motor sound");
	Serial.println("  Check for smooth operation");
	delay(800);
	Serial.println();
  }

  analogWrite(MOTOR_PIN, 0);  // Stop motor
  Serial.println("✓ All fixed speed tests completed\n");
}

// Test 4: Pulse motor on/off
void testMotorPulse() {
  Serial.println("[TEST 4] Motor Pulse Test");
  Serial.println("-----------------------------------------");
  Serial.println("Pulsing motor: ON/OFF cycle\n");

  for (int pulse = 0; pulse < 5; pulse++) {
	// Motor ON
	analogWrite(MOTOR_PIN, 200);
	Serial.print("Pulse ");
	Serial.print(pulse + 1);
	Serial.println(": ON (200 speed)");
	delay(500);

	// Motor OFF
	analogWrite(MOTOR_PIN, 0);
	Serial.println("         OFF");
	delay(500);
  }

  Serial.println("\n✓ Motor should pulse 5 times");
  Serial.println("✓ Should hear distinct ON/OFF clicks\n");
}
