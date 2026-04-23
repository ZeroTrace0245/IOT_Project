// Pin definitions
#define EN_PIN   8   // Enable
#define STEP_PIN 9   // Step
#define DIR_PIN  10  // Direction

// Motor control variables
int stepDelayMicroseconds = 1000; // Default speed
bool motorEnabled = false;
bool motorDirection = LOW;        // LOW = forward, HIGH = reverse

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  digitalWrite(EN_PIN, HIGH); // Driver disabled initially
  Serial.begin(9600);
  Serial.println("Stepper Motor Control Ready!");
  Serial.println("Commands: START, STOP, DIR=LEFT, DIR=RIGHT, SPEED=xxxx");
}

void loop() {
  // Check for serial input
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.equalsIgnoreCase("START")) {
      motorEnabled = true;
      digitalWrite(EN_PIN, LOW); // Enable driver
      Serial.println("Motor started");
    }
    else if (command.equalsIgnoreCase("STOP")) {
      motorEnabled = false;
      digitalWrite(EN_PIN, HIGH); // Disable driver
      Serial.println("Motor stopped");
    }
    else if (command.startsWith("DIR=")) {
      if (command.endsWith("LEFT")) {
        motorDirection = LOW;
        Serial.println("Direction set to LEFT");
      } else if (command.endsWith("RIGHT")) {
        motorDirection = HIGH;
        Serial.println("Direction set to RIGHT");
      }
    }
    else if (command.startsWith("SPEED=")) {
      int newSpeed = command.substring(6).toInt();
      if (newSpeed > 100) { // prevent too small delays
        stepDelayMicroseconds = newSpeed;
        Serial.print("Speed set to ");
        Serial.println(stepDelayMicroseconds);
      } else {
        Serial.println("Invalid speed (must be >100)");
      }
    }
  }

  // Run motor if enabled
  if (motorEnabled) {
    digitalWrite(DIR_PIN, motorDirection);
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelayMicroseconds);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelayMicroseconds);
  }
}
