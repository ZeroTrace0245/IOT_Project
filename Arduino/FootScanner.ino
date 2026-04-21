// NEMA 11 Stepper Motor Foot Scanner Controller
// Lead Screw: T8 (8mm per rotation)
// Steps per revolution: 200 (1.8 degree)
// Distance per step: 8mm / 200 = 0.04mm

const int stepPin = 3;
const int dirPin = 4;
const int limitSwitch = 2;

long totalSteps = 0;
bool isScanning = false;

void setup() {
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(limitSwitch, INPUT_PULLUP);
}

void loop() {
  if (Serial.available() > 0) {
	string command = Serial.readStringUntil('\n');
	if (command.startsWith("MEASURE")) {
	  startScan();
	} else if (command == "STOP") {
	  isScanning = false;
	}
  }

  if (isScanning) {
	// Check limit switch for stop condition or home position
	if (digitalRead(limitSwitch) == LOW) {
	  stopScan();
	  return;
	}

	// Step motor
	digitalWrite(stepPin, HIGH);
	delayMicroseconds(2500); // Adjustable via speed commands
	digitalWrite(stepPin, LOW);
	delayMicroseconds(2500);

	totalSteps++;

	// Send periodic updates: FOOT=mm
	if (totalSteps % 20 == 0) {
	  float distanceMm = totalSteps * 0.04;
	  Serial.print("FOOT=");
	  Serial.println(distanceMm);
	}
  }
}

void startScan() {
  totalSteps = 0;
  isScanning = true;
  digitalWrite(dirPin, HIGH); // Forward direction
}

void stopScan() {
  isScanning = false;
  float finalDistance = totalSteps * 0.04;
  Serial.print("RESULT=");
  Serial.println(finalDistance);
}
