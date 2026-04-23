// Limit Switch Test - Pin 4
// Simple test to verify limit switch on Pin 4 and Ground

#define LIMIT_SWITCH_PIN 4

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("LIMIT SWITCH TEST - PIN 4");
  Serial.println("========================================\n");

  // Configure pin as input with pullup
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);

  Serial.println("Pin Configuration: OK");
  Serial.println("LIMIT_SWITCH_PIN: Pin 4");
  Serial.println("Pullup: ENABLED");
  Serial.println("Ground: Connected to Pin 4 circuit\n");

  Serial.println("Instructions:");
  Serial.println("1. Leave limit switch OPEN (not pressed)");
  Serial.println("2. You should see: HIGH");
  Serial.println("3. PRESS the limit switch");
  Serial.println("4. You should see: LOW");
  Serial.println("5. RELEASE the limit switch");
  Serial.println("6. You should see: HIGH again\n");

  Serial.println("========================================");
  Serial.println("Starting continuous monitoring...\n");
}

int previousState = -1;  // Track previous state to detect changes

void loop() {
  int currentState = digitalRead(LIMIT_SWITCH_PIN);

  // Only print when state changes
  if (currentState != previousState) {
	previousState = currentState;

	unsigned long currentTime = millis();
	Serial.print("[");
	Serial.print(currentTime);
	Serial.print("ms] Limit Switch State: ");

	if (currentState == HIGH) {
	  Serial.println("HIGH ✓ (Switch OPEN / NOT PRESSED)");
	} else {
	  Serial.println("LOW ✓ (Switch CLOSED / PRESSED)");
	}
  }

  delay(50);  // Small delay to avoid overwhelming serial
}
