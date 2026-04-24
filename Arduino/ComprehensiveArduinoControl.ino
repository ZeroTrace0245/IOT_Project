// COMPREHENSIVE ARDUINO CONTROL SYSTEM
// Full control of Arduino Uno from WinUI application
// Supports all pins, sensors, and actuators with real-time feedback

#include <Wire.h>

// ========== PIN DEFINITIONS ==========
#define LIMIT_SWITCH_PIN 4
#define HW201_OBSTACLE_PIN 6  // HW201 infrared obstacle sensor
#define ULTRASONIC_TRIG_PIN 10  // Ultrasonic sensor trigger
#define ULTRASONIC_ECHO_PIN 11  // Ultrasonic sensor echo
#define PRESSURE_SENSOR_PIN A0
#define ANALOG_IN_PIN A1

// ========== STEPPER MOTOR DRIVER PINS (TMC2209 or similar) ==========
#define EN_PIN   8   // Enable pin (active LOW)
#define STEP_PIN 9   // Step/Clock pin
#define DIR_PIN  10  // Direction pin

// ========== I2C ADDRESSES ==========
#define PRESSURE_SENSOR_I2C_ADDR 0x76

// ========== COMMAND BUFFER ==========
#define BUFFER_SIZE 128
char commandBuffer[BUFFER_SIZE];
int bufferIndex = 0;

// ========== SYSTEM STATE ==========
struct SystemState {
  bool stepperEnabled;           // Motor enabled/disabled
  int stepperSpeed;              // Speed in microseconds (step delay)
  bool stepperDirection;         // LOW = forward, HIGH = reverse
  bool limitSwitchTriggered;
  bool obstacleDetected;         // HW201 obstacle sensor
  int ultrasonicDistance;        // Ultrasonic distance in cm
  int pressureRaw;               // 0-1023 (raw analog)
  float pressureVoltage;         // 0-5V (converted)
  unsigned long uptime;          // milliseconds
  bool isRunning;
};

SystemState systemState = {
  false, 1000, LOW, false, false, 0, 0, 0.0, 0, true
};

// ========== MOTOR CONTROL VARIABLES ==========
int stepDelayMicroseconds = 1000; // Default speed
bool motorEnabled = false;
bool motorDirection = LOW;        // LOW = forward, HIGH = reverse

// ========== AUTOMATIC FOOT SCANNING STATE MACHINE ==========
enum ScanState {
  SCAN_IDLE = 0,           // Waiting for foot
  SCAN_PRESSURE_DETECTED = 1,  // Foot placed on scanner
  SCAN_MOVING_FORWARD = 2,     // Motor moving forward
  SCAN_MEASURING = 3,          // Ultrasonic detecting foot
  SCAN_LIMIT_REACHED = 4,      // End position reached
  SCAN_MOVING_REVERSE = 5,     // Motor returning to home
  SCAN_COMPLETE = 6            // Scan finished
};

ScanState currentScanState = SCAN_IDLE;
unsigned long scanStartTime = 0;
int pressureThreshold = 500;     // Threshold for detecting foot pressure
int ultrasonicThreshold = 15;    // Detection threshold in cm (15cm = foot present)
int scanProgress = 0;            // Progress 0-100%

// ========== SETUP ==========
void setup() {
  Serial.begin(9600);
  delay(500);

  // Initialize I2C
  Wire.begin();

  // Initialize GPIO pins
  initializePins();

  // Print startup banner
  printBanner();

  // Initialize system
  systemState.uptime = millis();
}

// ========== MAIN LOOP ==========
void loop() {
  // Read serial commands
  if (Serial.available()) {
	char incomingChar = Serial.read();

	// Process newline
	if (incomingChar == '\n' || incomingChar == '\r') {
	  if (bufferIndex > 0) {
		commandBuffer[bufferIndex] = '\0';
		processCommand(commandBuffer);
		bufferIndex = 0;
	  }
	}
	// Add to buffer
	else if (bufferIndex < BUFFER_SIZE - 1) {
	  commandBuffer[bufferIndex++] = incomingChar;
	}
  }

  // ========== AUTOMATIC FOOT SCANNING STATE MACHINE ==========
  updateAutoScan();

  // Run stepper motor if enabled (manual control)
  if (motorEnabled && currentScanState == SCAN_IDLE) {
	digitalWrite(DIR_PIN, motorDirection);
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(stepDelayMicroseconds);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(stepDelayMicroseconds);
  }

  // Update sensor readings periodically
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead >= 1000) {
	lastSensorRead = millis();
	updateSensorReadings();
  }

  // Update system uptime
  systemState.uptime = millis();

  delay(1);  // Reduced delay for better stepper performance
}

// ========== PIN INITIALIZATION ==========
void initializePins() {
  // Digital inputs
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(HW201_OBSTACLE_PIN, INPUT);       // HW201 infrared sensor
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);      // Ultrasonic echo pin

  // Stepper Motor Driver Pins
  pinMode(EN_PIN, OUTPUT);                  // Enable (active LOW)
  pinMode(STEP_PIN, OUTPUT);                // Step/Clock
  pinMode(DIR_PIN, OUTPUT);                 // Direction
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);     // Ultrasonic trigger pin

  // Set default states
  digitalWrite(EN_PIN, HIGH);               // Driver disabled initially
  digitalWrite(STEP_PIN, LOW);              // Step off
  digitalWrite(DIR_PIN, LOW);               // Direction forward
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);   // Trigger off
}

// ========== AUTOMATIC FOOT SCANNING STATE MACHINE ==========
void updateAutoScan() {
  // Read current sensor values
  int currentPressure = analogRead(PRESSURE_SENSOR_PIN);
  int limitSwitchState = digitalRead(LIMIT_SWITCH_PIN);
  int ultrasonicDist = getUltrasonicDistance();

  switch (currentScanState) {

    case SCAN_IDLE:
      // Waiting for foot to be placed (pressure sensor detects force)
      if (currentPressure > pressureThreshold) {
        Serial.println("[SCAN] 🔴 PRESSURE DETECTED - Starting foot scan...");
        currentScanState = SCAN_PRESSURE_DETECTED;
        scanStartTime = millis();
      }
      break;

    case SCAN_PRESSURE_DETECTED:
      // Foot detected, start moving motor forward
      Serial.println("[SCAN] Moving motor forward...");
      startMotorForScan();
      currentScanState = SCAN_MOVING_FORWARD;
      break;

    case SCAN_MOVING_FORWARD:
      // Motor moving forward, check if ultrasonic detects foot
      if (ultrasonicDist > 0 && ultrasonicDist <= ultrasonicThreshold) {
        Serial.print("[SCAN] 📏 Foot detected at ");
        Serial.print(ultrasonicDist);
        Serial.println(" cm - Starting measurement...");
        currentScanState = SCAN_MEASURING;
        sendResponse("SCAN_MEASURING_START");
      }

      // Continue moving forward until limit switch is released
      moveMotorForScan();

      if (limitSwitchState == HIGH) {  // Limit switch released = end of travel
        Serial.println("[SCAN] Limit switch released - End of scan position");
        currentScanState = SCAN_LIMIT_REACHED;
        stopMotorForScan();
      }
      break;

    case SCAN_MEASURING:
      // Actively measuring foot position
      if (ultrasonicDist > ultrasonicThreshold || ultrasonicDist <= 0) {
        // Foot no longer detected
        Serial.println("[SCAN] Foot measurement complete!");
        sendResponse("SCAN_MEASURING_COMPLETE");
      }

      // Continue moving forward
      moveMotorForScan();

      if (limitSwitchState == HIGH) {  // Limit switch released = end of travel
        Serial.println("[SCAN] Limit switch released - Starting return...");
        currentScanState = SCAN_LIMIT_REACHED;
        stopMotorForScan();
      }
      break;

    case SCAN_LIMIT_REACHED:
      // Reached end position, now reverse to home
      Serial.println("[SCAN] ↩️ Returning to home position...");
      reverseMotorForScan();
      currentScanState = SCAN_MOVING_REVERSE;
      break;

    case SCAN_MOVING_REVERSE:
      // Moving backward to home position
      moveMotorReverse();

      if (limitSwitchState == LOW) {  // Limit switch triggered = at home
        Serial.println("[SCAN] ✅ Reached home position - Scan complete!");
        stopMotorForScan();
        currentScanState = SCAN_COMPLETE;
        sendResponse("SCAN_COMPLETE");

        // Log scan statistics
        unsigned long scanDuration = millis() - scanStartTime;
        Serial.print("[SCAN] Duration: ");
        Serial.print(scanDuration / 1000);
        Serial.println(" seconds");

        // Auto-reset after 2 seconds
        delay(2000);
        currentScanState = SCAN_IDLE;
        Serial.println("[SCAN] Ready for next scan");
      }
      break;

    case SCAN_COMPLETE:
      // Scan finished, wait for next foot
      if (currentPressure < 300) {  // Pressure released
        currentScanState = SCAN_IDLE;
      }
      break;
  }
}

// Helper functions for automatic scanning
void startMotorForScan() {
  motorEnabled = true;
  motorDirection = LOW;  // Forward
  digitalWrite(EN_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
}

void moveMotorForScan() {
  // Execute one step forward
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(stepDelayMicroseconds);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(stepDelayMicroseconds);
}

void moveMotorReverse() {
  // Execute one step backward
  digitalWrite(DIR_PIN, HIGH);  // Reverse direction
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(stepDelayMicroseconds);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(stepDelayMicroseconds);
}

void stopMotorForScan() {
  motorEnabled = false;
  digitalWrite(EN_PIN, HIGH);  // Disable driver
}

int getUltrasonicDistance() {
  // Get current ultrasonic distance reading
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 30000);  // 30ms timeout
  int distance = duration * 0.034 / 2;

  if (distance < 0 || distance > 400) {
    return -1;
  }
  return distance;
}

void scanFootStart() {
  // Command to start automatic foot scanning
  if (currentScanState != SCAN_IDLE) {
    Serial.println("[SCAN] Error: Scan already in progress");
    return;
  }

  Serial.println("[SCAN] Automatic foot scanning enabled");
  Serial.println("[SCAN] Please place foot on scanner...");
  currentScanState = SCAN_IDLE;  // Ready to detect pressure
}

void scanFootStop() {
  // Command to stop automatic foot scanning
  stopMotorForScan();
  currentScanState = SCAN_IDLE;
  Serial.println("[SCAN] Foot scanning stopped");
}

void reportScanStatus() {
  // Report current scanning state
  Serial.print("[SCAN] Current state: ");

  switch (currentScanState) {
    case SCAN_IDLE:
      Serial.println("IDLE - Waiting for foot");
      break;
    case SCAN_PRESSURE_DETECTED:
      Serial.println("PRESSURE_DETECTED");
      break;
    case SCAN_MOVING_FORWARD:
      Serial.println("MOVING_FORWARD");
      break;
    case SCAN_MEASURING:
      Serial.println("MEASURING");
      break;
    case SCAN_LIMIT_REACHED:
      Serial.println("LIMIT_REACHED");
      break;
    case SCAN_MOVING_REVERSE:
      Serial.println("MOVING_REVERSE");
      break;
    case SCAN_COMPLETE:
      Serial.println("COMPLETE");
      break;
  }

  Serial.print("SCAN_STATE=");
  Serial.println(currentScanState);
}
void startMotor() {
  motorEnabled = true;
  systemState.stepperEnabled = true;
  digitalWrite(EN_PIN, LOW);                // Enable driver

  Serial.println("[MOTOR] Started");
  Serial.println("MOTOR_STATUS=RUNNING");
}

void stopMotor() {
  motorEnabled = false;
  systemState.stepperEnabled = false;
  digitalWrite(EN_PIN, HIGH);               // Disable driver

  Serial.println("[MOTOR] Stopped");
  Serial.println("MOTOR_STATUS=STOPPED");
}

void setMotorDirection(String dir) {
  if (dir.equalsIgnoreCase("LEFT") || dir.equalsIgnoreCase("0")) {
    motorDirection = LOW;
    systemState.stepperDirection = LOW;
    Serial.println("[MOTOR] Direction: LEFT (Forward)");
  } 
  else if (dir.equalsIgnoreCase("RIGHT") || dir.equalsIgnoreCase("1")) {
    motorDirection = HIGH;
    systemState.stepperDirection = HIGH;
    Serial.println("[MOTOR] Direction: RIGHT (Reverse)");
  }
}

void setMotorSpeed(int speed) {
  // Speed is in microseconds (step delay)
  // Typical range: 100 to 10000 microseconds
  if (speed > 100) {
    stepDelayMicroseconds = speed;
    systemState.stepperSpeed = speed;

    Serial.print("[MOTOR] Speed set to ");
    Serial.print(speed);
    Serial.println(" µs (microseconds)");
    Serial.print("MOTOR_SPEED=");
    Serial.println(speed);
  } else {
    Serial.println("[ERROR] Speed must be > 100 microseconds");
  }
}

// ========== DISTANCE-BASED MOTOR CONTROL ==========
// Lead screw: T8 (8mm pitch), Motor: NEMA 17 (200 steps/rev)
// Calculation: 8mm per rev / 200 steps = 0.04mm per step
// Therefore: 250 steps = 1cm

int cmToSteps(float cm) {
  // Convert centimeters to motor steps
  // 250 steps = 1 cm
  return (int)(cm * 250.0);
}

void moveMotorCM(float cm, bool direction) {
  // Move motor a specific distance in centimeters
  int steps = cmToSteps(cm);
  moveMotorSteps(steps, direction, cm);
}

void moveMotorSteps(int steps, bool direction, float cm = 0.0) {
  // Move motor a specific number of steps
  if (steps <= 0) {
    Serial.println("[ERROR] Steps must be > 0");
    return;
  }

  // Set direction
  digitalWrite(DIR_PIN, direction ? HIGH : LOW);
  systemState.stepperDirection = direction ? HIGH : LOW;

  // Enable motor
  digitalWrite(EN_PIN, LOW);
  motorEnabled = true;
  systemState.stepperEnabled = true;

  // Execute step pulses
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelayMicroseconds);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelayMicroseconds);
  }

  // Motor complete
  Serial.print("[MOTOR] Completed ");
  Serial.print(steps);
  Serial.print(" steps");
  if (cm > 0.0) {
    Serial.print(" (");
    Serial.print(cm);
    Serial.print(" cm)");
  }
  Serial.println();
  Serial.print("MOTOR_MOVE_COMPLETE=");
  Serial.println(steps);
}

void parseMoveCommand(String command) {
  // Parse commands like "5CM" or "1250" (steps)
  command.toUpperCase();

  // Check if it ends with "CM"
  if (command.endsWith("CM")) {
    // Extract numeric part
    String numStr = command.substring(0, command.length() - 2);
    float cm = numStr.toFloat();

    if (cm > 0) {
      Serial.print("[MOVE] Moving ");
      Serial.print(cm);
      Serial.println(" cm (Forward)");
      moveMotorCM(cm, false);  // false = forward
    } else {
      Serial.println("[ERROR] Distance must be > 0");
    }
  }
  else {
    // Assume it's a step count
    int steps = command.toInt();
    if (steps > 0) {
      Serial.print("[MOVE] Moving ");
      Serial.print(steps);
      Serial.println(" steps (Forward)");
      moveMotorSteps(steps, false);  // false = forward
    } else {
      Serial.println("[ERROR] Steps must be > 0");
    }
  }
}

void getMotorStatus() {
  Serial.println("[MOTOR] Status Report:");
  Serial.print("  Enabled: ");
  Serial.println(motorEnabled ? "YES" : "NO");
  Serial.print("  Direction: ");
  Serial.println(motorDirection == LOW ? "LEFT (Forward)" : "RIGHT (Reverse)");
  Serial.print("  Speed: ");
  Serial.print(stepDelayMicroseconds);
  Serial.println(" µs");

  Serial.print("MOTOR_STATUS_ENABLED=");
  Serial.println(motorEnabled ? "1" : "0");
  Serial.print("MOTOR_STATUS_DIRECTION=");
  Serial.println(motorDirection == LOW ? "0" : "1");
  Serial.print("MOTOR_STATUS_SPEED=");
  Serial.println(stepDelayMicroseconds);
}

// ========== COMMAND PROCESSING ==========
void processCommand(char* command) {
  // Echo command
  Serial.print("[CMD] ");
  Serial.println(command);

  // Parse command
  char* token = strtok(command, "=");

  if (token == NULL) {
	sendError("Invalid command format");
	return;
  }

  // Remove leading/trailing spaces
  trimString(token);

  // Parse value if present
  char* valueStr = strtok(NULL, "=");
  int value = 0;
  if (valueStr != NULL) {
	trimString(valueStr);
	value = atoi(valueStr);
  }

  // Execute commands
  if (strcmp(token, "MOTOR_SPEED") == 0) {
	setMotorSpeed(value);
  }
  else if (strcmp(token, "MOTOR_DIR") == 0) {
	// Parse direction string
	if (valueStr != NULL) {
	  setMotorDirection(String(valueStr));
	}
  }
  else if (strcmp(token, "MOTOR_START") == 0) {
	startMotor();
  }
  else if (strcmp(token, "MOTOR_STOP") == 0) {
	stopMotor();
  }
  else if (strcmp(token, "MOTOR_STATUS") == 0) {
	getMotorStatus();
  }
  else if (strcmp(token, "START") == 0) {
	startMotor();
  }
  else if (strcmp(token, "STOP") == 0) {
	stopMotor();
  }
	setStepperSpeed(value);
  }
  else if (strcmp(token, "LIMIT_CHECK") == 0) {
	checkLimitSwitch();
  }
  else if (strcmp(token, "PRESSURE_READ") == 0) {
	readPressure();
  }
  else if (strcmp(token, "ANALOG_READ") == 0) {
	readAnalog(value);
  }
  else if (strcmp(token, "DIGITAL_READ") == 0) {
	readDigital(value);
  }
  else if (strcmp(token, "OBSTACLE_CHECK") == 0) {
	checkObstacleSensor();
  }
  else if (strcmp(token, "ULTRASONIC_READ") == 0) {
	readUltrasonicSensor();
  }
  else if (strcmp(token, "DIGITAL_WRITE") == 0) {
	// Format: DIGITAL_WRITE=pin,value
	writeDigital(value);
  }
  else if (strcmp(token, "PWM_WRITE") == 0) {
	// Format: PWM_WRITE=pin,value
	writePWM(value);
  }
  else if (strcmp(token, "MOVE") == 0) {
	// Format: MOVE=5CM (or MOVE=1250 for steps)
	if (valueStr != NULL) {
	  parseMoveCommand(String(valueStr));
	}
  }
  else if (strcmp(token, "SCAN_START") == 0) {
	// Start automatic foot scanning
	scanFootStart();
  }
  else if (strcmp(token, "SCAN_STOP") == 0) {
	// Stop automatic foot scanning
	scanFootStop();
  }
  else if (strcmp(token, "SCAN_STATUS") == 0) {
	// Report current scan state
	reportScanStatus();
  }
  else if (strcmp(token, "STATUS") == 0) {
	printStatus();
  }
  else if (strcmp(token, "HELP") == 0) {
	printHelp();
  }
  else if (strcmp(token, "RESET") == 0) {
	resetSystem();
  }
  else if (strcmp(token, "SENSOR_SCAN") == 0) {
	sensorScan();
  }
  else if (strcmp(token, "HOME") == 0) {
	goHome();
  }
  else if (strcmp(token, "PING") == 0) {
	sendResponse("PONG");
  }
  else if (strcmp(token, "UPTIME") == 0) {
	printUptime();
  }
  else if (strcmp(token, "GET_STATE") == 0) {
	sendSystemState();
  }
  else {
	sendError("Unknown command");
  }
}

// ========== MOTOR CONTROL ==========
void setMotorSpeed(int speed) {
  if (speed < 0 || speed > 255) {
	sendError("Speed must be 0-255");
	return;
  }

  systemState.motorSpeed = speed;
  analogWrite(MOTOR_PWM_PIN, speed);

  Serial.print("[MOTOR] Speed set to: ");
  Serial.println(speed);
  sendResponse("MOTOR_SPEED_OK");
}

void setMotorDirection(int direction) {
  if (direction != 0 && direction != 1) {
	sendError("Direction must be 0 (reverse) or 1 (forward)");
	return;
  }

  systemState.motorDirection = (direction == 0) ? -1 : 1;
  digitalWrite(DIR_SIGNAL_PIN, direction);

  Serial.print("[MOTOR] Direction set to: ");
  Serial.println(direction == 1 ? "FORWARD" : "REVERSE");
  sendResponse("MOTOR_DIR_OK");
}

void setMotorEnable(int enable) {
  systemState.motorEnabled = (enable == 1);
  digitalWrite(ENABLE_PIN, enable == 0 ? HIGH : LOW);

  Serial.print("[MOTOR] ");
  Serial.println(enable == 1 ? "ENABLED" : "DISABLED");
  sendResponse(enable == 1 ? "MOTOR_ENABLED" : "MOTOR_DISABLED");
}

void stopMotor() {
  systemState.motorSpeed = 0;
  systemState.motorEnabled = false;
  analogWrite(MOTOR_PWM_PIN, 0);
  digitalWrite(ENABLE_PIN, HIGH);

  Serial.println("[MOTOR] STOPPED");
  sendResponse("MOTOR_STOPPED");
}

// ========== STEPPER CONTROL (Legacy - Driver handles this now) ==========
void sendStepPulses(int count) {
  // This function is now handled by the main stepper driver
  // To send pulses, use MOTOR_SPEED and MOTOR_START commands
  Serial.println("[STEPPER] Use MOTOR_START/STOP and MOTOR_SPEED to control stepper");
  Serial.println("Step pulses are now generated continuously by the driver");
}

void setStepperSpeed(int frequencyHz) {
  // Convert Hz to microsecond delay
  // Hz = 1,000,000 / (2 * delayMicroseconds)
  // So: delayMicroseconds = 500,000 / Hz

  if (frequencyHz < 1 || frequencyHz > 5000) {
	sendError("Frequency must be 1-5000 Hz");
	return;
  }

  int delayUs = 500000 / frequencyHz;
  setMotorSpeed(delayUs);
}

// ========== SENSOR READING ==========
void checkLimitSwitch() {
  int state = digitalRead(LIMIT_SWITCH_PIN);
  systemState.limitSwitchTriggered = (state == LOW);

  Serial.print("[LIMIT] Switch state: ");
  Serial.println(state == LOW ? "TRIGGERED" : "OPEN");

  Serial.print("LIMIT_STATE=");
  Serial.println(state);
}

void checkObstacleSensor() {
  int state = digitalRead(HW201_OBSTACLE_PIN);
  systemState.obstacleDetected = (state == LOW);

  Serial.print("[HW201] Obstacle sensor: ");
  Serial.println(state == LOW ? "OBSTACLE DETECTED!" : "CLEAR");

  Serial.print("OBSTACLE_STATE=");
  Serial.println(state);
}

void readUltrasonicSensor() {
  // Send trigger pulse
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  // Read echo pulse duration
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);

  // Calculate distance (duration * speed of sound / 2)
  int distance = duration * 0.034 / 2;

  // Constrain to valid range (0-400cm typical for HC-SR04)
  if (distance < 0 || distance > 400) {
    distance = -1;  // Invalid reading
  }

  systemState.ultrasonicDistance = distance;

  // Print formatted response
  Serial.print("[ULTRASONIC] Distance: ");
  if (distance == -1) {
    Serial.println("OUT OF RANGE");
    Serial.println("ULTRASONIC_DISTANCE=-1");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
    Serial.print("ULTRASONIC_DISTANCE=");
    Serial.println(distance);
  }
}

void readPressure() {
  // Read raw analog value from pressure sensor (A0)
  int rawValue = analogRead(PRESSURE_SENSOR_PIN);

  // Convert raw value to voltage (0-1023 maps to 0-5V)
  float voltage = rawValue * (5.0 / 1023.0);

  // Store raw value in system state
  systemState.pressureReading = rawValue;

  // Print formatted response
  Serial.print("[PRESSURE] Raw Value: ");
  Serial.print(rawValue);
  Serial.print("   Voltage: ");
  Serial.print(voltage);
  Serial.println("V");

  // Print machine-readable values
  Serial.print("PRESSURE_RAW=");
  Serial.println(rawValue);
  Serial.print("PRESSURE_VOLTAGE=");
  Serial.println(voltage);
}

void readAnalog(int pin) {
  if (pin < A0 || pin > A5) {
	sendError("Invalid analog pin");
	return;
  }

  int value = analogRead(pin);

  Serial.print("[ANALOG] Pin ");
  Serial.print(pin);
  Serial.print(" = ");
  Serial.println(value);
  Serial.print("ANALOG_");
  Serial.print(pin);
  Serial.print("=");
  Serial.println(value);
}

void readDigital(int pin) {
  if (pin < 0 || pin > 13) {
	sendError("Invalid digital pin");
	return;
  }

  int value = digitalRead(pin);

  Serial.print("[DIGITAL] Pin ");
  Serial.print(pin);
  Serial.print(" = ");
  Serial.println(value);
  Serial.print("DIGITAL_");
  Serial.print(pin);
  Serial.print("=");
  Serial.println(value);
}

void writeDigital(int pin) {
  // Parse pin and value: "pin,value"
  int commaPos = -1;
  for (int i = 0; commandBuffer[i] != '\0'; i++) {
	if (commandBuffer[i] == ',') {
	  commaPos = i;
	  break;
	}
  }

  if (commaPos == -1) {
	sendError("Format: DIGITAL_WRITE=pin,value");
	return;
  }

  int pin = atoi(&commandBuffer[16]);  // Skip "DIGITAL_WRITE="
  int value = atoi(&commandBuffer[commaPos + 1]);

  if (pin < 0 || pin > 13) {
	sendError("Invalid pin");
	return;
  }

  pinMode(pin, OUTPUT);
  digitalWrite(pin, value);

  Serial.print("[GPIO] Pin ");
  Serial.print(pin);
  Serial.print(" set to ");
  Serial.println(value);
  sendResponse("DIGITAL_WRITE_OK");
}

void writePWM(int pin) {
  // Parse pin and value: "pin,value"
  int commaPos = -1;
  for (int i = 0; commandBuffer[i] != '\0'; i++) {
	if (commandBuffer[i] == ',') {
	  commaPos = i;
	  break;
	}
  }

  if (commaPos == -1) {
	sendError("Format: PWM_WRITE=pin,value");
	return;
  }

  int pin = atoi(&commandBuffer[11]);  // Skip "PWM_WRITE="
  int value = atoi(&commandBuffer[commaPos + 1]);

  if (pin < 3 || pin > 11 || (pin != 3 && pin != 5 && pin != 6 && pin != 9 && pin != 10 && pin != 11)) {
	sendError("Invalid PWM pin");
	return;
  }

  if (value < 0 || value > 255) {
	sendError("PWM value must be 0-255");
	return;
  }

  analogWrite(pin, value);

  Serial.print("[PWM] Pin ");
  Serial.print(pin);
  Serial.print(" set to ");
  Serial.println(value);
  sendResponse("PWM_WRITE_OK");
}

// ========== SYSTEM COMMANDS ==========
void printStatus() {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║          SYSTEM STATUS REPORT             ║");
  Serial.println("╠════════════════════════════════════════════╣");
  Serial.print("║ Motor Enabled: ");
  Serial.print(motorEnabled ? "YES" : "NO");
  Serial.println("                     ║");
  Serial.print("║ Motor Direction: ");
  Serial.print(motorDirection == LOW ? "LEFT (Forward)" : "RIGHT (Reverse)");
  Serial.println("  ║");
  Serial.print("║ Motor Speed: ");
  Serial.print(stepDelayMicroseconds);
  Serial.println(" µs            ║");
  Serial.print("║ Limit Switch: ");
  Serial.print(systemState.limitSwitchTriggered ? "TRIGGERED" : "OPEN");
  Serial.println("               ║");
  Serial.print("║ Obstacle Detected: ");
  Serial.print(systemState.obstacleDetected ? "YES" : "NO");
  Serial.println("              ║");
  Serial.print("║ Ultrasonic Distance: ");
  if (systemState.ultrasonicDistance == -1) {
    Serial.println("OUT OF RANGE        ║");
  } else {
    Serial.print(systemState.ultrasonicDistance);
    Serial.println(" cm               ║");
  }
  Serial.print("║ Pressure (Raw): ");
  Serial.print(systemState.pressureRaw);
  Serial.println("                   ║");
  Serial.print("║ Pressure (Voltage): ");
  Serial.print(systemState.pressureVoltage);
  Serial.println("V                ║");
  Serial.print("║ Uptime: ");
  Serial.print(systemState.uptime / 1000);
  Serial.println(" seconds                ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
}

void printHelp() {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║         COMMAND REFERENCE GUIDE            ║");
  Serial.println("╠════════════════════════════════════════════╣");
  Serial.println("║ STEPPER MOTOR CONTROL:                     ║");
  Serial.println("║  START                  (Start motor)      ║");
  Serial.println("║  STOP                   (Stop motor)       ║");
  Serial.println("║  MOTOR_START            (Start motor)      ║");
  Serial.println("║  MOTOR_STOP             (Stop motor)       ║");
  Serial.println("║  MOTOR_DIR=LEFT|RIGHT   (Set direction)    ║");
  Serial.println("║  MOTOR_SPEED=xxx        (Set speed µs)     ║");
  Serial.println("║  MOTOR_STATUS           (Get status)       ║");
  Serial.println("║  MOVE=5CM               (Move 5cm)         ║");
  Serial.println("║  MOVE=1250              (Move 1250 steps)   ║");
  Serial.println("║                                             ║");
  Serial.println("║ AUTOMATIC FOOT SCANNING:                   ║");
  Serial.println("║  SCAN_START             (Start scanning)   ║");
  Serial.println("║  SCAN_STOP              (Stop scanning)    ║");
  Serial.println("║  SCAN_STATUS            (Get scan state)   ║");
  Serial.println("║                                             ║");
  Serial.println("║ SENSOR READING:                            ║");
  Serial.println("║  LIMIT_CHECK            (Read limit)       ║");
  Serial.println("║  OBSTACLE_CHECK         (HW201 sensor)     ║");
  Serial.println("║  ULTRASONIC_READ        (Distance sensor)  ║");
  Serial.println("║  PRESSURE_READ          (Read pressure)    ║");
  Serial.println("║  ANALOG_READ=pin        (Read analog)      ║");
  Serial.println("║  DIGITAL_READ=pin       (Read digital)     ║");
  Serial.println("║                                             ║");
  Serial.println("║ GPIO CONTROL:                              ║");
  Serial.println("║  DIGITAL_WRITE=pin,val  (Write digital)    ║");
  Serial.println("║  PWM_WRITE=pin,val      (Write PWM)        ║");
  Serial.println("║                                             ║");
  Serial.println("║ SYSTEM COMMANDS:                           ║");
  Serial.println("║  STATUS                 (Show status)      ║");
  Serial.println("║  GET_STATE              (JSON state)       ║");
  Serial.println("║  UPTIME                 (System uptime)    ║");
  Serial.println("║  HOME                   (Go to home)       ║");
  Serial.println("║  RESET                  (Reset system)     ║");
  Serial.println("║  HELP                   (This help)        ║");
  Serial.println("║  PING                   (Echo test)        ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
}

void resetSystem() {
  stopMotor();
  systemState.motorSpeed = 0;
  systemState.motorDirection = 1;
  systemState.motorEnabled = false;
  systemState.stepperEnabled = false;

  Serial.println("[SYSTEM] RESET");
  sendResponse("SYSTEM_RESET");
}

void sensorScan() {
  Serial.println("\n[SCAN] Starting sensor scan...");

  // Check Limit Switch
  int limitState = digitalRead(LIMIT_SWITCH_PIN);
  Serial.print("[SCAN] Limit Switch (Pin 4): ");
  Serial.println(limitState == LOW ? "FOUND" : "OPEN");

  // Check I2C devices
  Serial.println("[SCAN] Scanning I2C bus...");
  for (int addr = 0; addr < 128; addr++) {
	Wire.beginTransmission(addr);
	if (Wire.endTransmission() == 0) {
	  Serial.print("[SCAN] Found I2C device at 0x");
	  Serial.println(addr, HEX);
	}
  }

  // Check analog sensors
  Serial.println("[SCAN] Checking analog inputs...");
  for (int pin = A0; pin <= A5; pin++) {
	int value = analogRead(pin);
	if (value > 0 && value < 1023) {
	  Serial.print("[SCAN] Analog pin A");
	  Serial.print(pin - A0);
	  Serial.print(": ");
	  Serial.println(value);
	}
  }

  Serial.println("[SCAN] Scan complete\n");
}

void goHome() {
  Serial.println("[HOME] Moving to home position...");

  // Enable motor and move toward limit switch
  motorEnabled = true;
  motorDirection = LOW;  // Move in one direction
  digitalWrite(EN_PIN, LOW);  // Enable driver

  int maxSteps = 10000;
  int stepCount = 0;

  while (digitalRead(LIMIT_SWITCH_PIN) == HIGH && stepCount < maxSteps) {
	digitalWrite(DIR_PIN, motorDirection);
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(stepDelayMicroseconds);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(stepDelayMicroseconds);
	stepCount++;
  }

  motorEnabled = false;
  digitalWrite(EN_PIN, HIGH);  // Disable driver

  Serial.print("[HOME] Complete - ");
  Serial.print(stepCount);
  Serial.println(" steps");
  sendResponse("HOME_COMPLETE");
}

void printUptime() {
  Serial.print("UPTIME=");
  Serial.println(systemState.uptime / 1000);
}

void sendSystemState() {
  Serial.print("STATE={");
  Serial.print("\"motorEnabled\":");
  Serial.print(motorEnabled ? "true" : "false");
  Serial.print(",\"motorDirection\":");
  Serial.print(motorDirection == LOW ? "\"LEFT\"" : "\"RIGHT\"");
  Serial.print(",\"motorSpeed\":");
  Serial.print(stepDelayMicroseconds);
  Serial.print(",\"limitTriggered\":");
  Serial.print(systemState.limitSwitchTriggered ? "true" : "false");
  Serial.print(",\"obstacleDetected\":");
  Serial.print(systemState.obstacleDetected ? "true" : "false");
  Serial.print(",\"ultrasonicDistance\":");
  Serial.print(systemState.ultrasonicDistance);
  Serial.print(",\"pressureRaw\":");
  Serial.print(systemState.pressureRaw);
  Serial.print(",\"pressureVoltage\":");
  Serial.print(systemState.pressureVoltage);
  Serial.print(",\"uptime\":");
  Serial.print(systemState.uptime / 1000);
  Serial.println("}");
}

// ========== HELPER FUNCTIONS ==========
void updateSensorReadings() {
  systemState.limitSwitchTriggered = (digitalRead(LIMIT_SWITCH_PIN) == LOW);
  systemState.obstacleDetected = (digitalRead(HW201_OBSTACLE_PIN) == LOW);

  // Read pressure sensor with voltage conversion
  int rawValue = analogRead(PRESSURE_SENSOR_PIN);
  systemState.pressureRaw = rawValue;
  systemState.pressureVoltage = rawValue * (5.0 / 1023.0);

  // Update ultrasonic reading
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;
  if (distance < 0 || distance > 400) {
    distance = -1;
  }
  systemState.ultrasonicDistance = distance;
}

void trimString(char* str) {
  // Remove leading spaces
  while (*str == ' ') str++;
  // Remove trailing spaces
  char* end = str + strlen(str) - 1;
  while (end > str && *end == ' ') end--;
  *(end + 1) = '\0';
}

void sendResponse(const char* response) {
  Serial.print("[RESPONSE] ");
  Serial.println(response);
}

void sendError(const char* error) {
  Serial.print("[ERROR] ");
  Serial.println(error);
}

void printBanner() {
  Serial.println("\n╔════════════════════════════════════════════════════════════╗");
  Serial.println("║       ARDUINO UNO - COMPREHENSIVE CONTROL SYSTEM           ║");
  Serial.println("║                                                            ║");
  Serial.println("║  Motor Control     │ Stepper Motor      │ Sensor Reading  ║");
  Serial.println("║  ─────────────────────────────────────────────────────── ║");
  Serial.println("║  • Speed (0-255)   │ • Pulse Control    │ • Limit Switch  ║");
  Serial.println("║  • Direction       │ • Frequency (Hz)   │ • Pressure      ║");
  Serial.println("║  • Enable/Disable  │ • Home Positioning │ • Analog Input  ║");
  Serial.println("║                                                            ║");
  Serial.println("║  GPIO Control: Digital Write, PWM Write, Read All Pins    ║");
  Serial.println("║  System: Status, Help, Reset, Sensor Scan, Home, Uptime  ║");
  Serial.println("║                                                            ║");
  Serial.println("║  Type 'HELP' for command reference                        ║");
  Serial.println("║  Baud Rate: 9600                                           ║");
  Serial.println("╚════════════════════════════════════════════════════════════╝\n");

  Serial.println("[READY] System initialized and ready for commands");
  Serial.println("[INFO] Waiting for command input...\n");
}
