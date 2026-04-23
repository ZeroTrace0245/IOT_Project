// COMPREHENSIVE ARDUINO CONTROL SYSTEM
// Full control of Arduino Uno from WinUI application
// Supports all pins, sensors, and actuators with real-time feedback

#include <Wire.h>

// ========== PIN DEFINITIONS ==========
#define LIMIT_SWITCH_PIN 4
#define MOTOR_PWM_PIN 8
#define STEP_SIGNAL_PIN 3
#define DIR_SIGNAL_PIN 2
#define ENABLE_PIN 5
#define PRESSURE_SENSOR_PIN A0
#define ANALOG_IN_PIN A1
#define HW201_OBSTACLE_PIN 6  // HW201 infrared obstacle sensor

// ========== I2C ADDRESSES ==========
#define PRESSURE_SENSOR_I2C_ADDR 0x76

// ========== COMMAND BUFFER ==========
#define BUFFER_SIZE 128
char commandBuffer[BUFFER_SIZE];
int bufferIndex = 0;

// ========== SYSTEM STATE ==========
struct SystemState {
  bool motorEnabled;
  int motorSpeed;        // 0-255
  int motorDirection;    // 1 = forward, -1 = reverse
  bool stepperEnabled;
  int stepperFrequency;  // Hz
  bool limitSwitchTriggered;
  bool obstacleDetected; // HW201 obstacle sensor
  int pressureReading;   // 0-1023
  unsigned long uptime;  // milliseconds
  bool isRunning;
};

SystemState systemState = {
  false, 0, 1, false, 0, false, false, 0, 0, true
};

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

  // Update sensor readings periodically
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead >= 1000) {
	lastSensorRead = millis();
	updateSensorReadings();
  }

  // Update system uptime
  systemState.uptime = millis();

  delay(10);
}

// ========== PIN INITIALIZATION ==========
void initializePins() {
  // Digital inputs
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(HW201_OBSTACLE_PIN, INPUT);  // HW201 infrared sensor

  // Digital outputs
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(STEP_SIGNAL_PIN, OUTPUT);
  pinMode(DIR_SIGNAL_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  // Set default states
  digitalWrite(MOTOR_PWM_PIN, LOW);
  digitalWrite(STEP_SIGNAL_PIN, LOW);
  digitalWrite(DIR_SIGNAL_PIN, LOW);
  digitalWrite(ENABLE_PIN, HIGH);  // Disabled by default
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
	setMotorDirection(value);
  }
  else if (strcmp(token, "MOTOR_ENABLE") == 0) {
	setMotorEnable(value);
  }
  else if (strcmp(token, "MOTOR_STOP") == 0) {
	stopMotor();
  }
  else if (strcmp(token, "STEP_PULSE") == 0) {
	sendStepPulses(value);
  }
  else if (strcmp(token, "STEP_SPEED") == 0) {
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
  else if (strcmp(token, "DIGITAL_WRITE") == 0) {
	// Format: DIGITAL_WRITE=pin,value
	writeDigital(value);
  }
  else if (strcmp(token, "PWM_WRITE") == 0) {
	// Format: PWM_WRITE=pin,value
	writePWM(value);
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

// ========== STEPPER CONTROL ==========
void sendStepPulses(int count) {
  if (count <= 0) {
	sendError("Step count must be > 0");
	return;
  }

  for (int i = 0; i < count; i++) {
	digitalWrite(STEP_SIGNAL_PIN, HIGH);
	delayMicroseconds(2);
	digitalWrite(STEP_SIGNAL_PIN, LOW);
	delayMicroseconds(500);
  }

  Serial.print("[STEPPER] Sent ");
  Serial.print(count);
  Serial.println(" pulses");
  sendResponse("STEP_PULSES_OK");
}

void setStepperSpeed(int frequencyHz) {
  if (frequencyHz < 1 || frequencyHz > 1000) {
	sendError("Frequency must be 1-1000 Hz");
	return;
  }

  systemState.stepperFrequency = frequencyHz;

  Serial.print("[STEPPER] Speed set to: ");
  Serial.print(frequencyHz);
  Serial.println(" Hz");
  sendResponse("STEPPER_SPEED_OK");
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

void readPressure() {
  Wire.beginTransmission(PRESSURE_SENSOR_I2C_ADDR);
  byte error = Wire.endTransmission();

  if (error == 0) {
	int pressure = analogRead(PRESSURE_SENSOR_PIN);
	systemState.pressureReading = pressure;

	Serial.print("[PRESSURE] Reading: ");
	Serial.println(pressure);
	Serial.print("PRESSURE=");
	Serial.println(pressure);
  } else {
	sendError("Pressure sensor not found");
  }
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
  Serial.print("║ Motor Speed: ");
  Serial.print(systemState.motorSpeed);
  Serial.println("                        ║");
  Serial.print("║ Motor Direction: ");
  Serial.print(systemState.motorDirection == 1 ? "FORWARD" : "REVERSE");
  Serial.println("               ║");
  Serial.print("║ Motor Enabled: ");
  Serial.print(systemState.motorEnabled ? "YES" : "NO");
  Serial.println("                     ║");
  Serial.print("║ Limit Switch: ");
  Serial.print(systemState.limitSwitchTriggered ? "TRIGGERED" : "OPEN");
  Serial.println("               ║");
  Serial.print("║ Obstacle Detected: ");
  Serial.print(systemState.obstacleDetected ? "YES" : "NO");
  Serial.println("              ║");
  Serial.print("║ Pressure: ");
  Serial.print(systemState.pressureReading);
  Serial.println("                         ║");
  Serial.print("║ Uptime: ");
  Serial.print(systemState.uptime / 1000);
  Serial.println(" seconds                ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
}

void printHelp() {
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║         COMMAND REFERENCE GUIDE            ║");
  Serial.println("╠════════════════════════════════════════════╣");
  Serial.println("║ MOTOR CONTROL:                             ║");
  Serial.println("║  MOTOR_SPEED=0-255      (Set speed)        ║");
  Serial.println("║  MOTOR_DIR=0|1          (Direction)        ║");
  Serial.println("║  MOTOR_ENABLE=0|1       (Enable/Disable)   ║");
  Serial.println("║  MOTOR_STOP             (Emergency stop)   ║");
  Serial.println("║                                             ║");
  Serial.println("║ STEPPER CONTROL:                           ║");
  Serial.println("║  STEP_PULSE=count       (Send pulses)      ║");
  Serial.println("║  STEP_SPEED=Hz          (Set frequency)    ║");
  Serial.println("║                                             ║");
  Serial.println("║ SENSOR READING:                            ║");
  Serial.println("║  LIMIT_CHECK            (Read limit)       ║");
  Serial.println("║  OBSTACLE_CHECK         (HW201 sensor)     ║");
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
  Serial.println("║  SENSOR_SCAN            (Find sensors)     ║");
  Serial.println("║  HOME                   (Go home)          ║");
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

  // Move stepper until limit switch is triggered
  int maxSteps = 10000;
  int stepCount = 0;

  while (digitalRead(LIMIT_SWITCH_PIN) == HIGH && stepCount < maxSteps) {
	digitalWrite(STEP_SIGNAL_PIN, HIGH);
	delayMicroseconds(2);
	digitalWrite(STEP_SIGNAL_PIN, LOW);
	delayMicroseconds(500);
	stepCount++;
  }

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
  Serial.print("\"motorSpeed\":");
  Serial.print(systemState.motorSpeed);
  Serial.print(",\"motorDir\":");
  Serial.print(systemState.motorDirection);
  Serial.print(",\"motorEnabled\":");
  Serial.print(systemState.motorEnabled ? "true" : "false");
  Serial.print(",\"limitTriggered\":");
  Serial.print(systemState.limitSwitchTriggered ? "true" : "false");
  Serial.print(",\"obstacleDetected\":");
  Serial.print(systemState.obstacleDetected ? "true" : "false");
  Serial.print(",\"pressure\":");
  Serial.print(systemState.pressureReading);
  Serial.print(",\"uptime\":");
  Serial.print(systemState.uptime / 1000);
  Serial.println("}");
}

// ========== HELPER FUNCTIONS ==========
void updateSensorReadings() {
  systemState.limitSwitchTriggered = (digitalRead(LIMIT_SWITCH_PIN) == LOW);
  systemState.obstacleDetected = (digitalRead(HW201_OBSTACLE_PIN) == LOW);
  systemState.pressureReading = analogRead(PRESSURE_SENSOR_PIN);
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
