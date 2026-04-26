// MOCK FOOT SCAN DATA SIMULATOR
// Simulates foot scanning data without physical scanner hardware

#define STEP_PIN 3
#define TEST_DELAY 1000

// Mock scan parameters
#define MOCK_SCAN_POINTS 100    // Number of scan points per foot
#define MOCK_SCAN_WIDTH 200     // Mock scan width (mm)
#define MOCK_SCAN_LENGTH 300    // Mock scan length (mm)
#define MOCK_SENSOR_RESOLUTION 256  // 8-bit sensor resolution (0-255)

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("FOOT SCAN DATA SIMULATOR");
  Serial.println("MOCK DATA GENERATOR");
  Serial.println("========================================\n");

  // No physical pin needed for simulation
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);

  Serial.println("Mode: MOCK DATA SIMULATION");
  Serial.println("Scan Points: " + String(MOCK_SCAN_POINTS));
  Serial.println("Scan Width: " + String(MOCK_SCAN_WIDTH) + "mm");
  Serial.println("Scan Length: " + String(MOCK_SCAN_LENGTH) + "mm");
  Serial.println("Resolution: " + String(MOCK_SENSOR_RESOLUTION) + " levels\n");

  Serial.println("This simulator generates realistic foot scan data");
  Serial.println("without requiring physical scanning hardware.\n");

  Serial.println("Data includes:");
  Serial.println("- Position coordinates (X, Y)");
  Serial.println("- Pressure/intensity values (0-255)");
  Serial.println("- Realistic pressure distribution\n");

  Serial.println("========================================");
  Serial.println("Starting MOCK SCAN data generation...\n");
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
}}
