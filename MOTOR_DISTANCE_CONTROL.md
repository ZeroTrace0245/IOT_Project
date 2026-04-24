# 🏃 Motor Distance Control Guide

## Overview
The IOT App now supports **precise distance-based motor control** using centimeter (cm) measurements instead of just raw step counts. This makes it intuitive to control the footwear scanning mechanism by specifying exact distances.

---

## ⚙️ Motor Specifications & Math

### Hardware Setup
- **Motor**: NEMA 17 Stepper Motor
- **Lead Screw**: T8 (8mm pitch per revolution)
- **Steps Per Revolution**: 200 (1.8° per step)
- **Driver**: TMC2209 or compatible

### Distance Calculation
```
Distance per step = Lead Screw Pitch / Steps per Revolution
				  = 8 mm / 200 steps
				  = 0.04 mm per step

Steps per centimeter = 10 mm / 0.04 mm per step
					 = 250 steps per cm
```

**Key Formula**: **250 steps = 1 cm travel**

---

## 🎮 Dashboard Motor Control

### Location
Navigate to: **Home** → **Dashboard** (default view)

### Motor Distance Control Panel
Located in the left column under "Live Sensor Feed"

#### Controls:
1. **Distance Input (cm)**
   - Enter value: 0.1 to 50 cm
   - Default: 5 cm

2. **Calculated Steps Display**
   - Automatically updates as distance changes
   - Shows: `[distance] × 250 = [steps]`
   - Example: 5 cm = 1250 steps

3. **Direction Selection**
   - **Forward**: Move in positive direction
   - **Reverse**: Move backward

4. **Movement Time Estimate**
   - Auto-calculated based on distance
   - Formula: Time (s) = Distance (cm) / 0.8 cm/s
   - Based on default speed of 2500µs per step

5. **Action Buttons**
   - **Move Now**: Execute the movement
   - **Stop**: Stop motor immediately

---

## 📡 Serial Commands

### Command Format
```
MOVE=5CM        → Move 5 cm forward
MOVE=1250       → Move 1250 steps forward
MOVE=10CM       → Move 10 cm forward
```

### Command Parsing
The Arduino code automatically detects the format:
- If command ends with **"CM"** → Parse as centimeters
- If numeric only → Parse as step count

### Serial Monitor Usage
```
// Example 1: Move 5 cm
MOVE=5CM
→ Output: [MOVE] Moving 5 cm (Forward)
→ Output: [MOTOR] Completed 1250 steps (5 cm)

// Example 2: Move 2.5 cm
MOVE=2.5CM
→ Output: [MOVE] Moving 2.5 cm (Forward)
→ Output: [MOTOR] Completed 625 steps (2.5 cm)

// Example 3: Move 1250 steps
MOVE=1250
→ Output: [MOVE] Moving 1250 steps (Forward)
→ Output: [MOTOR] Completed 1250 steps
```

---

## 📊 Timing Reference

### Speed Calculation
- Default `stepDelayMicroseconds = 2500`
- Each step pulse = HIGH (2500µs) + LOW (2500µs) = 5000µs
- Steps per second = 1,000,000µs / 5000µs = **200 steps/second**
- Distance per second = 200 steps / 250 steps per cm = **0.8 cm/s**

### Movement Duration Examples
| Distance | Steps | Time @ 0.8cm/s |
|----------|-------|-----------------|
| 1 cm     | 250   | 1.25 s         |
| 2.5 cm   | 625   | 3.125 s        |
| 5 cm     | 1250  | 6.25 s         |
| 10 cm    | 2500  | 12.5 s         |
| 20 cm    | 5000  | 25 s           |
| 50 cm    | 12500 | 62.5 s         |

---

## 🔧 Code Implementation

### Arduino Functions

#### `cmToSteps(float cm)`
Converts centimeters to motor steps.
```cpp
int steps = cmToSteps(5.0);  // Returns: 1250
```

#### `moveMotorCM(float cm, bool direction)`
Moves motor a specific distance in centimeters.
```cpp
moveMotorCM(5.0, false);  // Move 5 cm forward
moveMotorCM(3.0, true);   // Move 3 cm reverse
```

#### `moveMotorSteps(int steps, bool direction, float cm = 0.0)`
Moves motor a specific number of steps.
```cpp
moveMotorSteps(1250, false);        // Move 1250 steps forward
moveMotorSteps(625, true, 2.5);     // Move 625 steps (2.5 cm) reverse
```

#### `parseMoveCommand(String command)`
Parses user input from serial command.
```
Input: "5CM"    → Executes moveMotorCM(5.0, false)
Input: "1250"   → Executes moveMotorSteps(1250, false)
```

### WinUI Event Handler

#### `MoveMotorDistance_Click()`
Handles the "Move Now" button click in the Dashboard.
```csharp
// Gets distance from NumberBox
// Gets direction from RadioButtons
// Sends command: MOVE=XCM to Arduino
// Updates estimated time display
```

#### `MotorDistanceBox_ValueChanged()`
Updates calculated steps and time estimate in real-time.
```csharp
// Triggered when user changes distance input
// Recalculates: steps = distance × 250
// Recalculates: time = distance / 0.8
```

---

## 🚀 Quick Start

### In the Dashboard:

1. **Set Distance**
   - Type or spin to set desired distance (cm)
   - Watch "Calculated Steps" update automatically

2. **Choose Direction**
   - Select "Forward" or "Reverse"

3. **Review Time Estimate**
   - Check estimated movement time

4. **Move Motor**
   - Click "Move Now" button
   - Watch status in "Last Reading" area

5. **Monitor Completion**
   - Motor automatically stops after movement
   - Status updates: `[MOTOR] Completed X steps (Y cm)`

---

## 📋 Foot Scanner Integration

### Automatic Distance Measurements

The foot scanner application integrates motor distance control for:
- **Automatic scanning**: Move motor precise distances during foot measurement
- **Calibration**: Move known distances to verify accuracy
- **Homing**: Return to home position (limit switch)

### Expected Workflow:
1. User places foot on scanner
2. Motor moves precise distance (e.g., 2 cm)
3. Ultrasonic sensor measures foot at that position
4. Motor continues to next position
5. Process repeats until foot fully scanned

---

## ⚠️ Safety & Limits

### Distance Constraints
- **Minimum**: 0.1 cm (25 steps)
- **Maximum**: 50 cm (12500 steps)
- **Invalid Input**: Shows error message

### Motor Protection
- Limit switch prevents over-travel
- `HOME` command moves to limit switch position
- Emergency `STOP` command available

### Speed Safety
- Default speed: 2500µs per step (safe)
- Minimum: 100µs per step (max speed)
- For distances > 10cm, motor automatically handles acceleration

---

## 🧪 Testing Commands

### Serial Monitor Test Cases

```
# Test 1: Basic 5cm movement
MOVE=5CM

# Test 2: Small movement (1cm)
MOVE=1CM

# Test 3: Large movement (20cm)
MOVE=20CM

# Test 4: Very small (0.5cm = 125 steps)
MOVE=0.5CM

# Test 5: Step-based (direct step count)
MOVE=2500

# Test 6: Get motor status
MOTOR_STATUS

# Test 7: Set speed before moving (optional)
MOTOR_SPEED=2500
MOVE=5CM
```

---

## 🐛 Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| Motor doesn't move | Motor not enabled | Click "Move Now" or ensure power is on |
| Inaccurate distance | Motor stalled or slipping | Check motor power, verify step count |
| Motor moves wrong direction | Direction setting reversed | Toggle "Forward/Reverse" option |
| Incomplete movement | Hit limit switch | Increase distance or move opposite direction |
| Time estimate wrong | Speed changed | Check current speed with `MOTOR_STATUS` |

---

## 📚 Additional Resources

### Related Commands
- `MOTOR_SPEED=XXX` - Set motor speed in microseconds
- `MOTOR_DIR=LEFT|RIGHT` - Set direction before continuous movement
- `MOTOR_STATUS` - Check current speed and direction
- `START` - Begin continuous movement
- `STOP` - Stop all motor activity
- `HOME` - Move to limit switch (home position)

### Hardware Documentation
- TMC2209 Stepper Driver: Pin EN=8, STEP=9, DIR=10
- NEMA 17 Motor: 200 steps/revolution, 5V logic
- T8 Lead Screw: 8mm pitch, 2mm per revolution (with 4:1 reduction)

---

## ✅ Verification Checklist

- [ ] Arduino compiled successfully
- [ ] Motor moves when "Move Now" button clicked
- [ ] Distance input accepts values 0.1-50 cm
- [ ] Calculated steps display updates in real-time
- [ ] Time estimate changes with distance
- [ ] Motor stops after movement completes
- [ ] Dashboard shows movement confirmation
- [ ] "Stop" button halts motor immediately
- [ ] Reverse direction works correctly
- [ ] MOVE commands work in serial monitor

---

**Version**: 1.0  
**Last Updated**: 2024  
**Author**: Smart Footwear Scanner Project
