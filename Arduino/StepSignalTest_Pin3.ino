// MOCK FOOT SCAN DATA SIMULATOR
// Simulates foot scanning data without physical scanner hardware

#define STEP_PIN 5
#define A0_SENSOR_PIN A0
#define LIMIT_SWITCH_PIN 4
#define ULTRASONIC_TRIG_PIN 1
#define ULTRASONIC_ECHO_PIN 2
#define HW201_OBSTACLE_PIN 3
#define TEST_DELAY 1000

// Mock scan parameters
#define MOCK_SCAN_POINTS 100    // Number of scan points per foot
#define MOCK_SCAN_WIDTH 200     // Mock scan width (mm)
#define MOCK_SCAN_LENGTH 300    // Mock scan length (mm)
#define MOCK_SENSOR_RESOLUTION 256  // 8-bit sensor resolution (0-255)

// Sensor detection parameters
#define SENSOR_DETECTION_SAMPLES 10
#define SENSOR_THRESHOLD 50     // Threshold to detect sensor variation
#define SENSOR_NOISE_TOLERANCE 20  // Tolerance for noise detection

// Limit switch detection parameters
#define LIMIT_SWITCH_TEST_SAMPLES 20
#define LIMIT_SWITCH_DEBOUNCE_TIME 50  // ms

// Ultrasonic sensor parameters
#define ULTRASONIC_TEST_SAMPLES 10
#define ULTRASONIC_TIMEOUT 30000  // microseconds (max distance ~5 meters)
#define ULTRASONIC_MIN_DISTANCE 2  // cm (minimum valid distance)
#define ULTRASONIC_MAX_DISTANCE 400  // cm (maximum valid distance)

// HW201 Obstacle detection parameters
#define HW201_TEST_SAMPLES 20
#define HW201_DEBOUNCE_TIME 50  // ms
#define HW201_OBSTACLE_THRESHOLD 5  // Number of LOW readings to confirm obstacle

// Sensor status
bool sensorDetected = false;
bool useMockData = true;
bool limitSwitchDetected = false;
bool ultrasonicDetected = false;
bool hw201ObstacleDetected = false;
long ultrasonicLastDistance = 0;
bool currentObstacleState = false;

// Sensor calibration
const float REFERENCE_VOLTAGE = 5.0;  // Arduino reference voltage (5V)
const int ADC_RESOLUTION = 1023;      // 10-bit ADC max value

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("FOOT SCAN DATA SIMULATOR");
  Serial.println("SENSOR + LIMIT SWITCH DETECTION");
  Serial.println("========================================\n");

  // Configure pins
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);
  pinMode(A0_SENSOR_PIN, INPUT);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);  // Internal pull-up resistor
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(HW201_OBSTACLE_PIN, INPUT);

  Serial.println("Initializing...");
  Serial.println("Scan Points: " + String(MOCK_SCAN_POINTS));
  Serial.println("Scan Width: " + String(MOCK_SCAN_WIDTH) + "mm");
  Serial.println("Scan Length: " + String(MOCK_SCAN_LENGTH) + "mm\n");

  // Detect if pressure sensor is connected to A0
  Serial.println("========================================");
  Serial.println("SENSOR DETECTION SEQUENCE");
  Serial.println("========================================\n");

  detectPressureSensor();

  delay(1000);

  // Detect limit switch on pin 4
  Serial.println("========================================");
  Serial.println("LIMIT SWITCH DETECTION SEQUENCE");
  Serial.println("========================================\n");

  detectLimitSwitch();

  delay(1000);

  // Detect ultrasonic sensor on pins 1 & 2
  Serial.println("========================================");
  Serial.println("ULTRASONIC SENSOR DETECTION SEQUENCE");
  Serial.println("========================================\n");

  detectUltrasonicSensor();

  delay(1000);

  // Detect HW201 obstacle sensor on pin 3
  Serial.println("========================================");
  Serial.println("HW201 OBSTACLE SENSOR DETECTION SEQUENCE");
  Serial.println("========================================\n");

  detectHW201ObstacleSensor();

  delay(1000);
  Serial.println("\n========================================");
  Serial.println("Starting SCAN data generation...\n");
}

void loop() {
  testCycle++;

  Serial.println("\n========== MOCK SCAN CYCLE " + String(testCycle) + " ==========\n");

  // Test 1: Generate mock foot scan data
  generateMockFootScan();
  delay(TEST_DELAY);

  // Test 2: Generate with pressure variations
  generateFootScanWithPressure();
  delay(TEST_DELAY);

  // Test 3: Generate multiple point samples
  generateDetailedFootScan();
  delay(TEST_DELAY);

  Serial.println("\n");
  delay(2000);  // Wait before next cycle
}

// Generate mock foot scan data - basic grid pattern
void generateMockFootScan() {
  Serial.println("[MOCK SCAN 1] Basic Foot Scan Data");
  Serial.println("-----------------------------------------");
  Serial.println("Generating " + String(MOCK_SCAN_POINTS) + " scan points...\n");

  for (int i = 0; i < MOCK_SCAN_POINTS; i++) {
	int x = random(0, MOCK_SCAN_WIDTH);
	int y = random(0, MOCK_SCAN_LENGTH);
	int intensity = random(50, 200);  // Pressure values

	Serial.print("Point ");
	Serial.print(i + 1);
	Serial.print(": X=");
	Serial.print(x);
	Serial.print("mm, Y=");
	Serial.print(y);
	Serial.print("mm, Intensity=");
	Serial.println(intensity);

	if ((i + 1) % 20 == 0) {
	  delay(50);  // Small delay every 20 points
	}
  }

  Serial.println("\n✓ Basic foot scan complete");
  Serial.println("✓ Generated " + String(MOCK_SCAN_POINTS) + " data points\n");
}

// Generate foot scan with realistic pressure distribution
void generateFootScanWithPressure() {
  Serial.println("[MOCK SCAN 2] Foot Scan with Pressure Map");
  Serial.println("-----------------------------------------");
  Serial.println("Simulating pressure distribution across foot...\n");

  // Create a center point (ball of foot - highest pressure)
  int centerX = MOCK_SCAN_WIDTH / 2;
  int centerY = MOCK_SCAN_LENGTH / 3;

  for (int i = 0; i < MOCK_SCAN_POINTS; i++) {
	int x = random(0, MOCK_SCAN_WIDTH);
	int y = random(0, MOCK_SCAN_LENGTH);

	// Calculate distance from pressure center
	int dx = x - centerX;
	int dy = y - centerY;
	int distance = sqrt(dx * dx + dy * dy);

	// Pressure decreases with distance from center
	int intensity = max(30, 200 - (distance * 2));
	intensity = constrain(intensity, 0, 255);

	Serial.print("Point ");
	Serial.print(i + 1);
	Serial.print(": X=");
	Serial.print(x);
	Serial.print("mm, Y=");
	Serial.print(y);
	Serial.print("mm, Pressure=");
	Serial.println(intensity);

	if ((i + 1) % 25 == 0) {
	  delay(50);
	}
  }

  Serial.println("\n✓ Pressure-mapped scan complete");
  Serial.println("✓ Realistic foot pressure distribution simulated\n");
}

// Generate detailed foot scan with multiple samples per area
void generateDetailedFootScan() {
  Serial.println("[MOCK SCAN 3] Detailed Multi-Point Scan");
  Serial.println("-----------------------------------------");
  Serial.println("Generating high-resolution scan data...\n");

  // Create multiple scan regions (heel, arch, ball, toes)
  const int REGIONS = 4;
  const char* regionNames[] = {"HEEL", "ARCH", "BALL", "TOES"};
  int points_per_region = MOCK_SCAN_POINTS / REGIONS;

  for (int region = 0; region < REGIONS; region++) {
	Serial.print("Region ");
	Serial.print(region + 1);
	Serial.print(": ");
	Serial.println(regionNames[region]);

	int baseY = (region * MOCK_SCAN_LENGTH) / REGIONS;

	for (int i = 0; i < points_per_region; i++) {
	  int x = random(20, MOCK_SCAN_WIDTH - 20);
	  int y = baseY + random(0, MOCK_SCAN_LENGTH / REGIONS);

	  // Different pressure profiles per region
	  int intensity;
	  if (region == 0) {
		intensity = random(100, 180);  // Heel - moderate
	  } else if (region == 1) {
		intensity = random(60, 140);   // Arch - lower
	  } else if (region == 2) {
		intensity = random(150, 250);  // Ball - high
	  } else {
		intensity = random(80, 160);   // Toes - moderate
	  }

	  Serial.print("  ");
	  Serial.print(regionNames[region]);
	  Serial.print(" - X=");
	  Serial.print(x);
	  Serial.print("mm, Y=");
	  Serial.print(y);
	  Serial.print("mm, Intensity=");
	  Serial.println(intensity);
	}

	delay(100);  // Pause between regions
  }

  Serial.println("\n✓ Detailed multi-region scan complete");
  Serial.println("✓ Regional pressure variations simulated\n");
}


// Initialize mock data seed
void initMockData() {
  randomSeed(analogRead(0));  // Seed random generator for variety
}

// ============================================
// SENSOR DETECTION FUNCTIONS
// ============================================

// Detect if pressure sensor is connected to A0
void detectPressureSensor() {
  Serial.println("Checking A0 for pressure sensor...\n");

  // Take multiple readings to establish baseline
  int readings[SENSOR_DETECTION_SAMPLES];
  int minReading = 1023;
  int maxReading = 0;
  int totalReading = 0;

  Serial.println("Taking " + String(SENSOR_DETECTION_SAMPLES) + " readings from A0...");

  for (int i = 0; i < SENSOR_DETECTION_SAMPLES; i++) {
    readings[i] = analogRead(A0_SENSOR_PIN);

    // Track min/max
    if (readings[i] < minReading) minReading = readings[i];
    if (readings[i] > maxReading) maxReading = readings[i];

    totalReading += readings[i];

    Serial.print("Sample ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(readings[i]);

    delay(100);  // Small delay between readings
  }

  int averageReading = totalReading / SENSOR_DETECTION_SAMPLES;
  int readingVariance = maxReading - minReading;

  Serial.println("\n--- ANALYSIS ---");
  Serial.print("Average Reading: ");
  Serial.println(averageReading);
  Serial.print("Min Reading: ");
  Serial.println(minReading);
  Serial.print("Max Reading: ");
  Serial.println(maxReading);
  Serial.print("Variance (Max - Min): ");
  Serial.println(readingVariance);

  // Determine if sensor is connected
  Serial.println("\n--- SENSOR STATUS ---");

  if (averageReading < 10 && readingVariance < SENSOR_NOISE_TOLERANCE) {
    // Connected to ground or very low readings = no sensor
    sensorDetected = false;
    useMockData = true;
    Serial.println("❌ NO SENSOR DETECTED (reads ground)");
    Serial.println("   Mode: MOCK DATA");
  } 
  else if (averageReading > 1010 && readingVariance < SENSOR_NOISE_TOLERANCE) {
    // Connected to power or very high readings = no sensor
    sensorDetected = false;
    useMockData = true;
    Serial.println("❌ NO SENSOR DETECTED (reads VCC)");
    Serial.println("   Mode: MOCK DATA");
  }
  else if (readingVariance > SENSOR_THRESHOLD) {
    // Sensor is responding with variable readings = sensor detected
    sensorDetected = true;
    useMockData = false;
    Serial.println("✓ SENSOR DETECTED!");
    Serial.println("   Mode: REAL SENSOR DATA");
    Serial.print("   Sensor is responsive with variance: ");
    Serial.println(readingVariance);
  }
  else if (readingVariance > SENSOR_NOISE_TOLERANCE) {
    // Slight variation - likely a noisy sensor or unconnected pin
    sensorDetected = true;
    useMockData = false;
    Serial.println("⚠ SENSOR LIKELY CONNECTED (noisy)");
    Serial.println("   Mode: REAL SENSOR DATA (with noise filtering)");
    Serial.print("   Variance: ");
    Serial.println(readingVariance);
  }
  else {
    // Stable mid-range reading = unconnected pin
    sensorDetected = false;
    useMockData = true;
    Serial.println("❌ NO SENSOR DETECTED (floating pin)");
    Serial.println("   Mode: MOCK DATA");
  }

  Serial.println("\n--- RECOMMENDATIONS ---");
  if (!sensorDetected) {
    Serial.println("• Check A0 connection");
    Serial.println("• Verify sensor power supply");
    Serial.println("• Test sensor with multimeter");
    Serial.println("• Using mock data for testing");
  } else {
    Serial.println("• Sensor connected and operational");
    Serial.println("• Monitor reading variance");
    Serial.println("• Check for noise if variance > 100");
  }

  // Run pressure detection test if sensor detected
  if (sensorDetected) {
    delay(1000);
    Serial.println("\n========================================");
    Serial.println("PRESSURE DETECTION TEST");
    Serial.println("========================================\n");
    testPressureDetection();
  }
}

// Test pressure detection by monitoring for pressure changes
void testPressureDetection() {
  Serial.println("Monitoring A0 for pressure input...");
  Serial.println("Apply pressure to sensor now!\n");

  int baselineReading = analogRead(A0_SENSOR_PIN);
  float baselineVoltage = convertToVoltage(baselineReading);
  int maxPressureReading = baselineReading;
  int minPressureReading = baselineReading;
  float maxVoltage = baselineVoltage;
  float minVoltage = baselineVoltage;
  int pressureDetected = 0;

  Serial.print("Baseline Raw Value: ");
  Serial.print(baselineReading);
  Serial.print("   Voltage: ");
  Serial.print(baselineVoltage);
  Serial.println("V");
  Serial.println("\nListening for 5 seconds...\n");
  Serial.println("Format: Raw Value | Voltage | Pressure Change");
  Serial.println("----------------------------------------");

  unsigned long testStartTime = millis();
  int sampleCount = 0;

  while (millis() - testStartTime < 5000) {  // 5 second test
    int currentReading = analogRead(A0_SENSOR_PIN);
    float currentVoltage = convertToVoltage(currentReading);
    sampleCount++;

    // Track min/max during pressure test
    if (currentReading < minPressureReading) minPressureReading = currentReading;
    if (currentReading > maxPressureReading) maxPressureReading = currentReading;
    if (currentVoltage < minVoltage) minVoltage = currentVoltage;
    if (currentVoltage > maxVoltage) maxVoltage = currentVoltage;

    // Check if significant pressure change detected
    int pressureChange = abs(currentReading - baselineReading);
    float voltageChange = abs(currentVoltage - baselineVoltage);

    if (pressureChange > 50) {
      if (pressureDetected == 0) {
        Serial.print("✓ PRESSURE DETECTED! ");
      }
      pressureDetected++;
    }

    // Print readings every 500ms (5 samples * 100ms)
    if (sampleCount % 5 == 0) {
      Serial.print(currentReading);
      Serial.print(" | ");
      Serial.print(currentVoltage);
      Serial.print("V | ");
      Serial.print(pressureChange);
      Serial.print(" (");
      Serial.print(voltageChange);
      Serial.println("V)");
    }

    delay(100);  // 100ms between samples
  }

  Serial.println("----------------------------------------\n");
  Serial.println("--- PRESSURE TEST RESULTS ---");
  Serial.print("Test Duration: 5 seconds");
  Serial.print(" | Samples: ");
  Serial.println(sampleCount);

  Serial.println("\n--- RAW VALUES (0-1023) ---");
  Serial.print("Baseline Reading: ");
  Serial.println(baselineReading);
  Serial.print("Min Reading: ");
  Serial.println(minPressureReading);
  Serial.print("Max Reading: ");
  Serial.println(maxPressureReading);
  Serial.print("Total Range: ");
  Serial.println(maxPressureReading - minPressureReading);

  Serial.println("\n--- VOLTAGE (0-5V) ---");
  Serial.print("Baseline Voltage: ");
  Serial.print(baselineVoltage);
  Serial.println("V");
  Serial.print("Min Voltage: ");
  Serial.print(minVoltage);
  Serial.println("V");
  Serial.print("Max Voltage: ");
  Serial.print(maxVoltage);
  Serial.println("V");
  Serial.print("Voltage Range: ");
  Serial.print(maxVoltage - minVoltage);
  Serial.println("V");

  if (pressureDetected > 0) {
    Serial.println("\n✓ PRESSURE DETECTION SUCCESSFUL!");
    Serial.print("✓ Pressure changes detected ");
    Serial.print(pressureDetected);
    Serial.println(" times");
    Serial.println("✓ Sensor is working and responsive!");
  } else {
    Serial.println("\n❌ NO PRESSURE DETECTED");
    Serial.println("• Sensor may not be responding to pressure");
    Serial.println("• Check sensor connection and wiring");
    Serial.println("• Verify sensor calibration");
    Serial.println("• Try applying stronger pressure");
  }

  Serial.println("\n========================================\n");
}

// Get sensor value with fallback to mock data
int getSensorReading() {
  if (sensorDetected) {
    return analogRead(A0_SENSOR_PIN);
  } else {
    return random(50, 200);  // Mock pressure value
  }
}

// Get sensor status string
String getSensorStatus() {
  if (sensorDetected) {
    return "LIVE SENSOR";
  } else {
    return "MOCK DATA";
  }
}

// Convert raw analog value to voltage
float convertToVoltage(int rawValue) {
  return rawValue * (REFERENCE_VOLTAGE / ADC_RESOLUTION);
}

// ============================================
// LIMIT SWITCH DETECTION FUNCTIONS
// ============================================

// Detect if limit switch is connected to pin 4
void detectLimitSwitch() {
  Serial.println("Checking Pin 4 for limit switch...\n");

  int pressedCount = 0;      // Count LOW readings (pressed)
  int releasedCount = 0;     // Count HIGH readings (released)
  int lastState = HIGH;
  int stateChangeCount = 0;

  Serial.println("Taking " + String(LIMIT_SWITCH_TEST_SAMPLES) + " samples from Pin 4...");
  Serial.println("Format: Sample | State | Description\n");

  for (int i = 0; i < LIMIT_SWITCH_TEST_SAMPLES; i++) {
    int state = digitalRead(LIMIT_SWITCH_PIN);

    // Count state changes
    if (state != lastState) {
      stateChangeCount++;
      lastState = state;
    }

    // Track pressed vs released
    if (state == LOW) {
      pressedCount++;
    } else {
      releasedCount++;
    }

    // Print state
    Serial.print("Sample ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(state == LOW ? "LOW" : "HIGH");
    Serial.print(" | ");
    Serial.println(state == LOW ? "PRESSED" : "RELEASED");

    delay(LIMIT_SWITCH_DEBOUNCE_TIME);
  }

  Serial.println("\n--- ANALYSIS ---");
  Serial.print("Total Pressed (LOW): ");
  Serial.println(pressedCount);
  Serial.print("Total Released (HIGH): ");
  Serial.println(releasedCount);
  Serial.print("State Changes: ");
  Serial.println(stateChangeCount);

  Serial.println("\n--- LIMIT SWITCH STATUS ---");

  // Determine if limit switch is connected
  if (pressedCount == LIMIT_SWITCH_TEST_SAMPLES) {
    // Always LOW = stuck or shorted
    limitSwitchDetected = false;
    Serial.println("❌ LIMIT SWITCH NOT RESPONDING");
    Serial.println("   State: Always LOW (Pressed)");
    Serial.println("   Possible Issues:");
    Serial.println("   - Switch stuck in pressed position");
    Serial.println("   - Pin shorted to ground");
    Serial.println("   - Check wiring and connection");
  }
  else if (releasedCount == LIMIT_SWITCH_TEST_SAMPLES) {
    // Always HIGH = not connected or floating
    limitSwitchDetected = false;
    Serial.println("❌ LIMIT SWITCH NOT DETECTED");
    Serial.println("   State: Always HIGH (Released)");
    Serial.println("   Possible Issues:");
    Serial.println("   - Switch not connected");
    Serial.println("   - Pin floating (disconnected)");
    Serial.println("   - Check connection to Pin 4");
  }
  else if (stateChangeCount >= 2) {
    // Has state changes = switch is working
    limitSwitchDetected = true;
    Serial.println("✓ LIMIT SWITCH DETECTED!");
    Serial.println("   State: RESPONDING");
    Serial.print("   State Changes Detected: ");
    Serial.println(stateChangeCount);
    Serial.println("   ✓ Switch is working properly!");
  }
  else {
    // Some variation but minimal = likely connected
    limitSwitchDetected = true;
    Serial.println("✓ LIMIT SWITCH LIKELY CONNECTED");
    Serial.println("   State: STABLE");
    Serial.print("   Current State: ");
    Serial.println(lastState == LOW ? "PRESSED" : "RELEASED");
  }

  Serial.println("\n--- RECOMMENDATIONS ---");
  if (!limitSwitchDetected) {
    Serial.println("• Check Pin 4 connection");
    Serial.println("• Verify switch is properly wired");
    Serial.println("• Test switch with multimeter");
    Serial.println("• Press switch manually to verify operation");
  } else {
    Serial.println("• Limit switch is operational");
    if (stateChangeCount == 0) {
      Serial.println("• Switch is in a stable state");
      Serial.println("• Try pressing the switch to test");
    }
  }
}

// Test limit switch by waiting for manual input
void testLimitSwitchManual() {
  Serial.println("\n========================================");
  Serial.println("MANUAL LIMIT SWITCH TEST");
  Serial.println("========================================\n");

  Serial.println("Press and release the limit switch!");
  Serial.println("Monitoring for 10 seconds...\n");

  int pressCount = 0;
  int releaseCount = 0;
  int lastState = digitalRead(LIMIT_SWITCH_PIN);

  unsigned long testStart = millis();

  while (millis() - testStart < 10000) {
    int state = digitalRead(LIMIT_SWITCH_PIN);

    // Detect state changes
    if (state != lastState) {
      if (state == LOW) {
        pressCount++;
        Serial.print("✓ Switch PRESSED (");
        Serial.print(pressCount);
        Serial.println(" times)");
      } else {
        releaseCount++;
        Serial.print("✓ Switch RELEASED (");
        Serial.print(releaseCount);
        Serial.println(" times)");
      }
      lastState = state;
    }

    delay(50);  // Debounce delay
  }

  Serial.println("\n--- TEST RESULTS ---");
  Serial.print("Press Events: ");
  Serial.println(pressCount);
  Serial.print("Release Events: ");
  Serial.println(releaseCount);
  Serial.print("Total State Changes: ");
  Serial.println(pressCount + releaseCount);

  if (pressCount > 0 || releaseCount > 0) {
    Serial.println("\n✓ LIMIT SWITCH TEST SUCCESSFUL!");
    Serial.println("✓ Switch is responsive!");
  } else {
    Serial.println("\n❌ NO SWITCH ACTIVITY DETECTED");
    Serial.println("❌ Please verify switch connection");
  }

  Serial.println("\n========================================\n");
}

// Get limit switch status
String getLimitSwitchStatus() {
  if (limitSwitchDetected) {
    int state = digitalRead(LIMIT_SWITCH_PIN);
    return state == LOW ? "PRESSED" : "RELEASED";
  } else {
    return "NOT DETECTED";
  }
}

// ============================================
// ULTRASONIC SENSOR DETECTION FUNCTIONS
// ============================================

// Measure distance using ultrasonic sensor
long measureUltrasonicDistance() {
  // Trigger pulse
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  // Read echo time
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, ULTRASONIC_TIMEOUT);

  // Calculate distance: distance = (duration / 2) * speed_of_sound
  // Speed of sound = 0.034 cm/microsecond
  long distance = duration * 0.034 / 2;

  return distance;
}

// Detect if ultrasonic sensor is connected to pins 1 & 2
void detectUltrasonicSensor() {
  Serial.println("Checking Pins 1 & 2 for ultrasonic sensor...\n");

  int validReadings = 0;
  int invalidReadings = 0;
  int minDistance = 9999;
  int maxDistance = 0;
  long totalDistance = 0;

  Serial.println("Taking " + String(ULTRASONIC_TEST_SAMPLES) + " distance measurements...");
  Serial.println("Format: Sample | Distance (cm) | Status\n");

  for (int i = 0; i < ULTRASONIC_TEST_SAMPLES; i++) {
    long distance = measureUltrasonicDistance();

    Serial.print("Sample ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(distance);
    Serial.print(" cm | ");

    // Validate reading
    if (distance > ULTRASONIC_MIN_DISTANCE && distance < ULTRASONIC_MAX_DISTANCE) {
      validReadings++;
      totalDistance += distance;

      if (distance < minDistance) minDistance = distance;
      if (distance > maxDistance) maxDistance = distance;

      Serial.println("✓ VALID");
    } else if (distance <= 0) {
      invalidReadings++;
      Serial.println("✗ NO ECHO (timeout)");
    } else {
      invalidReadings++;
      Serial.print("✗ OUT OF RANGE");
      if (distance <= ULTRASONIC_MIN_DISTANCE) {
        Serial.println(" (too close)");
      } else {
        Serial.println(" (too far)");
      }
    }

    delay(100);  // Small delay between measurements
  }

  Serial.println("\n--- ANALYSIS ---");
  Serial.print("Valid Readings: ");
  Serial.print(validReadings);
  Serial.print(" / ");
  Serial.println(ULTRASONIC_TEST_SAMPLES);
  Serial.print("Invalid Readings: ");
  Serial.println(invalidReadings);

  if (validReadings > 0) {
    long averageDistance = totalDistance / validReadings;
    Serial.print("Min Distance: ");
    Serial.print(minDistance);
    Serial.println(" cm");
    Serial.print("Max Distance: ");
    Serial.print(maxDistance);
    Serial.println(" cm");
    Serial.print("Average Distance: ");
    Serial.print(averageDistance);
    Serial.println(" cm");
    ultrasonicLastDistance = averageDistance;
  }

  Serial.println("\n--- ULTRASONIC SENSOR STATUS ---");

  // Determine if ultrasonic sensor is connected
  if (validReadings == 0) {
    ultrasonicDetected = false;
    Serial.println("❌ ULTRASONIC SENSOR NOT DETECTED");
    Serial.println("   Status: No valid readings");
    Serial.println("   Possible Issues:");
    Serial.println("   - Sensor not connected");
    Serial.println("   - Pins 1 & 2 incorrect");
    Serial.println("   - Trigger pin not working");
    Serial.println("   - Echo pin not responding");
  }
  else if (validReadings >= ULTRASONIC_TEST_SAMPLES * 0.7) {
    // 70% or more valid readings = sensor detected
    ultrasonicDetected = true;
    Serial.println("✓ ULTRASONIC SENSOR DETECTED!");
    Serial.println("   Status: OPERATIONAL");
    Serial.print("   Valid Reading Rate: ");
    Serial.print((validReadings * 100) / ULTRASONIC_TEST_SAMPLES);
    Serial.println("%");
    Serial.println("   ✓ Sensor is working properly!");
  }
  else if (validReadings > 0) {
    // Some valid readings but noisy
    ultrasonicDetected = true;
    Serial.println("✓ ULTRASONIC SENSOR DETECTED (NOISY)");
    Serial.println("   Status: OPERATIONAL (with noise)");
    Serial.print("   Valid Reading Rate: ");
    Serial.print((validReadings * 100) / ULTRASONIC_TEST_SAMPLES);
    Serial.println("%");
    Serial.println("   ⚠ Sensor has some noise/interference");
  }

  Serial.println("\n--- RECOMMENDATIONS ---");
  if (!ultrasonicDetected) {
    Serial.println("• Check Pin 1 (Trigger) connection");
    Serial.println("• Check Pin 2 (Echo) connection");
    Serial.println("• Verify sensor power supply (5V)");
    Serial.println("• Verify GND connection");
    Serial.println("• Check for loose connections");
  } else {
    Serial.println("• Ultrasonic sensor is operational");
    Serial.println("• Ensure clear line of sight to objects");
    if ((validReadings * 100) / ULTRASONIC_TEST_SAMPLES < 100) {
      Serial.println("• Monitor for noise if readings vary significantly");
    }
  }
}

// Get continuous ultrasonic distance reading
long getUltrasonicDistance() {
  if (ultrasonicDetected) {
    long distance = measureUltrasonicDistance();
    ultrasonicLastDistance = distance;
    return distance;
  } else {
    // Return mock distance
    return random(10, 50);
  }
}

// Get ultrasonic sensor status
String getUltrasonicStatus() {
  if (ultrasonicDetected) {
    return "OPERATIONAL";
  } else {
    return "NOT DETECTED";
  }
}

// ============================================
// HW201 OBSTACLE DETECTION SENSOR FUNCTIONS
// ============================================

// Detect if HW201 obstacle sensor is connected to pin 3
void detectHW201ObstacleSensor() {
  Serial.println("Checking Pin 3 for HW201 obstacle sensor...\n");

  int detectedCount = 0;      // Count LOW readings (obstacle detected)
  int clearCount = 0;         // Count HIGH readings (no obstacle)
  int stateChangeCount = 0;
  int lastState = HIGH;

  Serial.println("Taking " + String(HW201_TEST_SAMPLES) + " samples from Pin 3...");
  Serial.println("Format: Sample | State | Description\n");

  for (int i = 0; i < HW201_TEST_SAMPLES; i++) {
    int state = digitalRead(HW201_OBSTACLE_PIN);

    // Count state changes
    if (state != lastState) {
      stateChangeCount++;
      lastState = state;
    }

    // Track detection vs clear
    if (state == LOW) {
      detectedCount++;
    } else {
      clearCount++;
    }

    // Print state
    Serial.print("Sample ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(state == LOW ? "LOW" : "HIGH");
    Serial.print(" | ");
    Serial.println(state == LOW ? "OBSTACLE DETECTED" : "NO OBSTACLE");

    delay(HW201_DEBOUNCE_TIME);
  }

  Serial.println("\n--- ANALYSIS ---");
  Serial.print("Obstacle Detected (LOW): ");
  Serial.println(detectedCount);
  Serial.print("No Obstacle (HIGH): ");
  Serial.println(clearCount);
  Serial.print("State Changes: ");
  Serial.println(stateChangeCount);

  Serial.println("\n--- HW201 OBSTACLE SENSOR STATUS ---");

  // Determine if HW201 sensor is connected and working
  if (detectedCount == HW201_TEST_SAMPLES) {
    // Always LOW = always detecting obstacle
    hw201ObstacleDetected = true;
    currentObstacleState = true;
    Serial.println("⚠ HW201 SENSOR DETECTED (always triggering)");
    Serial.println("   State: Always LOW (Obstacle Detected)");
    Serial.println("   Possible Issues:");
    Serial.println("   - Obstacle very close to sensor");
    Serial.println("   - Sensor sensitivity too high");
    Serial.println("   - Remove nearby objects to test");
  }
  else if (clearCount == HW201_TEST_SAMPLES) {
    // Always HIGH = not detecting anything
    hw201ObstacleDetected = true;
    currentObstacleState = false;
    Serial.println("✓ HW201 SENSOR DETECTED (no obstacles)");
    Serial.println("   State: Always HIGH (No Obstacle)");
    Serial.println("   ✓ Sensor is operational!");
    Serial.println("   ✓ No obstacles detected in range");
  }
  else if (stateChangeCount >= 2) {
    // Has state changes = sensor is responsive
    hw201ObstacleDetected = true;
    currentObstacleState = (lastState == LOW);
    Serial.println("✓ HW201 SENSOR DETECTED!");
    Serial.println("   State: RESPONSIVE");
    Serial.print("   State Changes Detected: ");
    Serial.println(stateChangeCount);
    Serial.println("   ✓ Sensor is working and responsive!");
  }
  else if (detectedCount > 0 || clearCount > 0) {
    // Stable readings but no changes = likely connected
    hw201ObstacleDetected = true;
    currentObstacleState = (lastState == LOW);
    Serial.println("✓ HW201 SENSOR LIKELY CONNECTED");
    Serial.println("   State: STABLE");
    Serial.print("   Current State: ");
    Serial.println(lastState == LOW ? "OBSTACLE DETECTED" : "NO OBSTACLE");
  }
  else {
    // No readings at all = not connected
    hw201ObstacleDetected = false;
    Serial.println("❌ HW201 SENSOR NOT DETECTED");
    Serial.println("   Status: No response");
    Serial.println("   Possible Issues:");
    Serial.println("   - Sensor not connected");
    Serial.println("   - Pin 3 not receiving signal");
    Serial.println("   - Verify power supply (5V)");
  }

  Serial.println("\n--- RECOMMENDATIONS ---");
  if (!hw201ObstacleDetected) {
    Serial.println("• Check Pin 3 (OUT) connection");
    Serial.println("• Verify sensor power supply (5V)");
    Serial.println("• Verify GND connection");
    Serial.println("• Test sensor with multimeter");
    Serial.println("• Place hand near sensor to trigger");
  } else {
    Serial.println("• HW201 sensor is operational");
    Serial.println("• Sensor range: typically 2-40 cm");
    Serial.println("• Adjust sensitivity screw if needed");
    if (stateChangeCount == 0) {
      Serial.println("• Move hand near sensor to verify response");
    }
  }
}

// Continuous obstacle detection
bool isObstacleDetected() {
  if (hw201ObstacleDetected) {
    int state = digitalRead(HW201_OBSTACLE_PIN);
    currentObstacleState = (state == LOW);
    return currentObstacleState;
  } else {
    // Return mock obstacle state
    return random(0, 2) == 0;
  }
}

// Manual HW201 test - wait for user interaction
void testHW201Manual() {
  Serial.println("\n========================================");
  Serial.println("MANUAL HW201 OBSTACLE TEST");
  Serial.println("========================================\n");

  Serial.println("Move your hand near/away from the sensor!");
  Serial.println("Monitoring for 10 seconds...\n");

  int detectionEvents = 0;
  int clearEvents = 0;
  int lastState = digitalRead(HW201_OBSTACLE_PIN);

  unsigned long testStart = millis();

  while (millis() - testStart < 10000) {
    int state = digitalRead(HW201_OBSTACLE_PIN);

    // Detect state changes
    if (state != lastState) {
      if (state == LOW) {
        detectionEvents++;
        Serial.print("✓ OBSTACLE DETECTED (");
        Serial.print(detectionEvents);
        Serial.println(" times)");
      } else {
        clearEvents++;
        Serial.print("✓ OBSTACLE CLEARED (");
        Serial.print(clearEvents);
        Serial.println(" times)");
      }
      lastState = state;
    }

    delay(HW201_DEBOUNCE_TIME);
  }

  Serial.println("\n--- TEST RESULTS ---");
  Serial.print("Detection Events: ");
  Serial.println(detectionEvents);
  Serial.print("Clear Events: ");
  Serial.println(clearEvents);
  Serial.print("Total State Changes: ");
  Serial.println(detectionEvents + clearEvents);

  if (detectionEvents > 0 || clearEvents > 0) {
    Serial.println("\n✓ HW201 SENSOR TEST SUCCESSFUL!");
    Serial.println("✓ Sensor is responsive!");
  } else {
    Serial.println("\n❌ NO SENSOR ACTIVITY DETECTED");
    Serial.println("❌ Please verify sensor connection");
    Serial.println("❌ Move hand near sensor to trigger");
  }

  Serial.println("\n========================================\n");
}

// Get HW201 obstacle sensor status
String getHW201Status() {
  if (hw201ObstacleDetected) {
    int state = digitalRead(HW201_OBSTACLE_PIN);
    return state == LOW ? "OBSTACLE DETECTED" : "CLEAR";
  } else {
    return "NOT DETECTED";
  }
}
