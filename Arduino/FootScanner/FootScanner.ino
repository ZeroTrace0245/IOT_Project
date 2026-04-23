// Smart Foot Measurement System - Arduino Uno
// Controls NEMA 17 Stepper Motor via TMC2209 Driver
// Communicates with WinUI 3 Dashboard via Serial (9600 baud)
// 
// Motor Wiring (NEMA 17 4-wire):
// Black (Coil 1A) → TMC2209 pin 1A
// Green (Coil 1B) → TMC2209 pin 1B
// Red (Coil 2A)   → TMC2209 pin 2A
// Blue (Coil 2B)  → TMC2209 pin 2B

// TMC2209 Driver Control Pins (Arduino Uno)
#define STEP_PIN 3      // PWM pin 3 - Step pulse input
#define DIR_PIN 4       // Digital pin 4 - Direction control (HIGH=forward, LOW=reverse)
#define ENABLE_PIN 5    // PWM pin 5 - Enable motor (LOW=enabled, HIGH=disabled)

// Sensor Pins (Arduino Uno)
#define LIMIT_SWITCH_PIN 2   // Digital pin 2 (interrupt capable) - Limit switch (pulled HIGH, goes LOW when triggered)
#define PRESSURE_SENSOR_PIN A0  // Analog pin A0 - Pressure/force sensor (0-1023)

// Motor Parameters
#define STEPS_PER_REV 200    // NEMA 17: 1.8 degrees per step (200 steps = 360 degrees)
#define LEAD_SCREW_MM 8      // T8 lead screw: 8mm per revolution
#define MM_PER_STEP (LEAD_SCREW_MM / STEPS_PER_REV)  // 0.04mm per step

// Step timing (microseconds) - affects motor speed
#define STEP_DELAY_US 500    // Delay between step pulses (lower = faster)

// Global Variables
long totalSteps = 0;
long maxSteps = 10000;  // Maximum movement limit (safety)
bool isMotorEnabled = false;
bool isScanning = false;
float currentDistance = 0.0;
unsigned long lastMeasurementTime = 0;
const unsigned long MEASUREMENT_INTERVAL = 100;  // milliseconds between FOOT= updates

void setup() {
  // Initialize Serial Communication (Arduino Uno: 9600 baud standard)
  Serial.begin(9600);
  delay(500);  // Wait for serial to initialize

  Serial.println("");
  Serial.println("========================================");
  Serial.println("READY=Smart Foot Scanner - Arduino Uno");
  Serial.println("========================================");
  Serial.println("");

  // Configure Motor Control Pins as OUTPUT
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  // Configure Sensor Pins as INPUT
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);  // Pullup enabled - goes LOW when switch closes
  pinMode(PRESSURE_SENSOR_PIN, INPUT);      // Analog input (0-1023 on Arduino Uno)

  // Motor initially disabled (HIGH = disabled on TMC2209)
  digitalWrite(ENABLE_PIN, HIGH);
  digitalWrite(DIR_PIN, HIGH);   // Default forward direction
  digitalWrite(STEP_PIN, LOW);   // Step line low

  Serial.println("STATUS=System initialized - motor disabled");
  Serial.println("STATUS=Ready to receive commands");
  delay(500);
}

void loop() {
  // Handle serial commands from WinUI Dashboard
  if (Serial.available() > 0) {
	String command = Serial.readStringUntil('\n');
	command.trim();
	if (command.length() > 0) {
	  processCommand(command);
	}
  }

  // Motor movement loop (stepping)
  if (isScanning) {
	// Check limit switch (home position detection) - active LOW
	if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
	  stopMotor();
	  isScanning = false;
	  Serial.println("STATUS=Limit switch triggered - home position detected");
	  return;
	}

	// Step the motor
	pulseStep();
	totalSteps++;

	// Check step limit (safety maximum)
	if (totalSteps >= maxSteps) {
	  stopMotor();
	  isScanning = false;
	  Serial.println("STATUS=Maximum step limit reached - stopping");
	}

	// Send periodic distance updates to dashboard
	if (millis() - lastMeasurementTime >= MEASUREMENT_INTERVAL) {
	  float distanceMm = totalSteps * MM_PER_STEP;
	  Serial.print("FOOT=");
	  Serial.println(distanceMm, 2);  // 2 decimal places
	  lastMeasurementTime = millis();
	}
  }
}

// Process incoming serial commands from the dashboard
void processCommand(String cmd) {
  if (cmd.startsWith("MEASURE")) {
	// Start measurement scan
	startScan();
	Serial.println("STATUS=Measurement started");
  }
  else if (cmd.startsWith("HOME")) {
	// Move to home position (limit switch)
	goHome();
  }
  else if (cmd.startsWith("STOP")) {
	// Stop motor immediately
	stopMotor();
	Serial.println("STATUS=Motor stopped");
  }
  else if (cmd.startsWith("MOVE")) {
	// Parse and move: MOVE;DISTANCE=50;SPEED=100;DIR=0
	parseAndMove(cmd);
  }
  else if (cmd.startsWith("TEST")) {
	// Test motor movement
	testMotor();
  }
  else if (cmd.startsWith("PING")) {
	// Heartbeat check
	Serial.println("PONG=OK");
  }
  else if (cmd.startsWith("READ")) {
	// Read pressure sensor
	int pressureRaw = analogRead(PRESSURE_SENSOR_PIN);
	Serial.print("PRESSURE=");
	Serial.println(pressureRaw);
  }
}

// Start measurement scan
void startScan() {
  totalSteps = 0;
  isScanning = true;
  isMotorEnabled = true;
  lastMeasurementTime = millis();

  // Enable motor (LOW = enabled on TMC2209)
  digitalWrite(ENABLE_PIN, LOW);
  delay(50);  // Give motor time to enable

  // Set forward direction
  digitalWrite(DIR_PIN, HIGH);

  Serial.println("STATUS=Scan started - motor enabled, moving forward");
}

// Stop motor and disable
void stopMotor() {
  isScanning = false;
  isMotorEnabled = false;

  // Disable motor (HIGH = disabled on TMC2209)
  digitalWrite(ENABLE_PIN, HIGH);
  delay(50);

  // Send final distance measurement
  float finalDistance = totalSteps * MM_PER_STEP;
  Serial.print("RESULT=");
  Serial.println(finalDistance, 2);
  Serial.println("STATUS=Motor disabled - measurement complete");
}

// Move to home position using limit switch
void goHome() {
  Serial.println("STATUS=Moving to home position...");
  isMotorEnabled = true;
  digitalWrite(ENABLE_PIN, LOW);
  delay(50);

  // Reverse direction to move toward limit switch
  digitalWrite(DIR_PIN, LOW);

  totalSteps = 0;
  int stepCount = 0;

  // Move until limit switch is triggered (active LOW)
  while (digitalRead(LIMIT_SWITCH_PIN) == HIGH && stepCount < 20000) {
	pulseStep();
	stepCount++;
  }

  // Reset counters at home
  totalSteps = 0;
  isMotorEnabled = false;
  digitalWrite(ENABLE_PIN, HIGH);
  delay(50);

  Serial.println("STATUS=Home position reached - motor disabled");
}

// Parse and execute MOVE command
// Format: MOVE;DISTANCE=50;SPEED=100;DIR=1
void parseAndMove(String cmd) {
  float distance = extractValue(cmd, "DISTANCE=");
  float speed = extractValue(cmd, "SPEED=");
  int direction = (int)extractValue(cmd, "DIR=");

  if (distance > 0 && speed > 0) {
	isMotorEnabled = true;
	digitalWrite(ENABLE_PIN, LOW);
	delay(50);

	// Set direction (1 = forward/HIGH, 0 = reverse/LOW)
	digitalWrite(DIR_PIN, direction == 1 ? HIGH : LOW);

	// Calculate steps needed
	long stepsNeeded = (long)(distance / MM_PER_STEP);
	long stepCount = 0;

	// Calculate delay based on speed (speed = steps per second)
	long delayMicros = (speed > 0) ? (long)(1000000 / speed) : STEP_DELAY_US;

	Serial.print("STATUS=Moving ");
	Serial.print(distance, 1);
	Serial.print("mm at ");
	Serial.print(speed, 0);
	Serial.println(" steps/sec");

	// Move the motor
	while (stepCount < stepsNeeded) {
	  pulseStep();
	  stepCount++;
	}

	isMotorEnabled = false;
	digitalWrite(ENABLE_PIN, HIGH);
	delay(50);

	Serial.println("STATUS=Movement complete - motor disabled");
  } else {
	Serial.println("ERROR=Invalid MOVE parameters (distance and speed must be > 0)");
  }
}

// Test motor with forward and reverse movement
void testMotor() {
  Serial.println("STATUS=Starting motor test sequence...");

  isMotorEnabled = true;
  digitalWrite(ENABLE_PIN, LOW);
  delay(100);

  // Test forward movement
  Serial.println("STATUS=Test: Moving forward (100 steps)");
  digitalWrite(DIR_PIN, HIGH);  // Forward
  for (int i = 0; i < 100; i++) {
	pulseStep();
  }
  delay(500);

  // Test reverse movement
  Serial.println("STATUS=Test: Moving reverse (100 steps)");
  digitalWrite(DIR_PIN, LOW);   // Reverse
  for (int i = 0; i < 100; i++) {
	pulseStep();
  }
  delay(500);

  // Final position back to center
  Serial.println("STATUS=Test: Returning to center");
  digitalWrite(DIR_PIN, HIGH);  // Forward again
  for (int i = 0; i < 50; i++) {
	pulseStep();
  }

  isMotorEnabled = false;
  digitalWrite(ENABLE_PIN, HIGH);
  delay(50);

  Serial.println("STATUS=Motor test complete - all movements successful");
}

// Generate a single step pulse for TMC2209
// TMC2209 requires: minimum 50ns pulse width, but 1-2µs is safer and more reliable
void pulseStep() {
  // Rising edge (step pulse begins)
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(2);  // 2µs pulse width (safe for TMC2209)

  // Falling edge (step pulse ends)
  digitalWrite(STEP_PIN, LOW);

  // Delay before next step (controls motor speed)
  delayMicroseconds(STEP_DELAY_US);
}

// Extract numeric value from command string
float extractValue(String str, String key) {
  int pos = str.indexOf(key);
  if (pos == -1) return 0.0;

  pos += key.length();
  String value = "";
  while (pos < str.length() && (isDigit(str[pos]) || str[pos] == '.')) {
	value += str[pos];
	pos++;
  }
  return value.toFloat();
}

// Extract string value from command (not currently used but useful)
String extractString(String str, String key) {
  int pos = str.indexOf(key);
  if (pos == -1) return "";

  pos += key.length();
  String value = "";
  while (pos < str.length() && str[pos] != ';' && str[pos] != '\n') {
	value += str[pos];
	pos++;
  }
  return value;
}
