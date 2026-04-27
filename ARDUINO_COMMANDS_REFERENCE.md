# Arduino Commands Reference for Sensor Detection

This document shows what Arduino commands the system expects and how to implement them in your Arduino sketch.

## Command Responses Required

### 1. **LIMIT_CHECK** - Limit Switch Detection
**Command:** `LIMIT_CHECK`  
**Expected Response:** `LIMIT_STATE=0` or `LIMIT_STATE=1`  
**Meaning:**
- `0` = Switch pressed (LOW)
- `1` = Switch released (HIGH)

**Arduino Implementation:**
```cpp
if (command == "LIMIT_CHECK") {
	int limitState = digitalRead(LIMIT_SWITCH_PIN); // Pin 4
	Serial.println("LIMIT_STATE=" + String(limitState));
}
```

---

### 2. **OBSTACLE_CHECK** - HW201 Obstacle Sensor Detection
**Command:** `OBSTACLE_CHECK`  
**Expected Response:** `OBSTACLE_STATE=0` or `OBSTACLE_STATE=1`  
**Meaning:**
- `0` = Obstacle detected (LOW)
- `1` = Clear, no obstacle (HIGH)

**Arduino Implementation:**
```cpp
if (command == "OBSTACLE_CHECK") {
	int obstacleState = digitalRead(OBSTACLE_PIN); // Pin 3
	Serial.println("OBSTACLE_STATE=" + String(obstacleState));
}
```

---

### 3. **ULTRASONIC_READ** - Ultrasonic Sensor Distance
**Command:** `ULTRASONIC_READ`  
**Expected Response:** `ULTRASONIC_DISTANCE=XX`  
**Meaning:** Distance in centimeters (2-400 cm range)

**Arduino Implementation:**
```cpp
if (command == "ULTRASONIC_READ") {
	int distance = readUltrasonicDistance(); // Your function
	Serial.println("ULTRASONIC_DISTANCE=" + String(distance));
}

// Example ultrasonic reading function
int readUltrasonicDistance() {
	// Assuming trigger on pin 1, echo on pin 2
	digitalWrite(TRIGGER_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIGGER_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER_PIN, LOW);

	long duration = pulseIn(ECHO_PIN, HIGH);
	int distance = duration * 0.034 / 2;
	return distance;
}
```

---

### 4. **PRESSURE_READ** - Pressure Sensor Value
**Command:** `PRESSURE_READ`  
**Expected Response:** `PRESSURE_RAW=XXX`  
**Meaning:** Raw ADC value (0-1023 for 5V max)

**Arduino Implementation:**
```cpp
if (command == "PRESSURE_READ") {
	int pressureValue = analogRead(PRESSURE_PIN); // Pin A0
	Serial.println("PRESSURE_RAW=" + String(pressureValue));
}
```

---

## Complete Arduino Command Handler Example

```cpp
// Define pins
#define LIMIT_SWITCH_PIN 4
#define OBSTACLE_PIN 3
#define TRIGGER_PIN 1
#define ECHO_PIN 2
#define PRESSURE_PIN A0

void handleSerialCommand(String command) {
	command.trim();
	command.toUpperCase();

	if (command == "LIMIT_CHECK") {
		int limitState = digitalRead(LIMIT_SWITCH_PIN);
		Serial.println("LIMIT_STATE=" + String(limitState));
	}
	else if (command == "OBSTACLE_CHECK") {
		int obstacleState = digitalRead(OBSTACLE_PIN);
		Serial.println("OBSTACLE_STATE=" + String(obstacleState));
	}
	else if (command == "ULTRASONIC_READ") {
		int distance = readUltrasonicDistance();
		Serial.println("ULTRASONIC_DISTANCE=" + String(distance));
	}
	else if (command == "PRESSURE_READ") {
		int pressureValue = analogRead(PRESSURE_PIN);
		Serial.println("PRESSURE_RAW=" + String(pressureValue));
	}
}

int readUltrasonicDistance() {
	digitalWrite(TRIGGER_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIGGER_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER_PIN, LOW);

	long duration = pulseIn(ECHO_PIN, HIGH);
	int distance = duration * 0.034 / 2;
	return distance;
}
```

---

## Integration in Your Arduino Loop

```cpp
void setup() {
	Serial.begin(9600);
	pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
	pinMode(OBSTACLE_PIN, INPUT);
	pinMode(TRIGGER_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);
}

void loop() {
	if (Serial.available()) {
		String command = Serial.readStringUntil('\n');
		handleSerialCommand(command);
	}
}
```

---

## Expected Handshake Sequence

When user clicks "Start Handshake" in Sensor Detection panel:

```
1. APP → LIMIT_CHECK
   ARDUINO → LIMIT_STATE=1

2. APP → OBSTACLE_CHECK
   ARDUINO → OBSTACLE_STATE=1

3. APP → ULTRASONIC_READ
   ARDUINO → ULTRASONIC_DISTANCE=45

4. APP → PRESSURE_READ
   ARDUINO → PRESSURE_RAW=523

5. APP shows: "✓ Handshake Complete (1250ms)"
			 "📊 Connected: 4/4 sensors"
```

---

## Important Notes

⚠️ **Response Format Matters:**
- Use **exact** response prefixes: `LIMIT_STATE=`, `OBSTACLE_STATE=`, `ULTRASONIC_DISTANCE=`, `PRESSURE_RAW=`
- Add value immediately after `=` with no extra spaces
- End response with newline (`println()` adds this)

✅ **Timeout Behavior:**
- Each sensor has 2-second timeout
- If no response within 2 seconds, sensor marked as "Not Detected"
- Ensure your Arduino responds quickly to commands

✅ **Value Ranges:**
- Limit Switch: 0 (pressed) or 1 (released)
- Obstacle: 0 (detected) or 1 (clear)
- Ultrasonic: Any positive integer (cm)
- Pressure: 0-1023 (ADC 8-bit or 10-bit)

---

## Testing Commands Manually

You can test these commands using the Arduino IDE Serial Monitor:

1. Open Serial Monitor (9600 baud)
2. Type: `LIMIT_CHECK`
3. Should see: `LIMIT_STATE=1` (or 0)

Same for other commands.

---

**Ready to integrate? Update your Arduino sketch with these commands and the sensor detection will work perfectly! ✅**
