# 🔍 HW201 INFRARED OBSTACLE DETECTION SENSOR - INTEGRATION GUIDE

## Overview

The **HW201 Infrared Obstacle Detection Sensor** has been fully integrated into your Arduino control system. This sensor can detect objects/obstacles in its field of view using infrared light.

---

## 📋 What is HW201?

**HW201 Features:**
- ✅ Infrared obstacle detection
- ✅ Detection range: 2-40 cm (configurable)
- ✅ Digital output (HIGH/LOW)
- ✅ Operating voltage: 3.3V - 5V
- ✅ Low power consumption
- ✅ Fast response time
- ✅ Easy to integrate

---

## 🔌 Wiring

### Pin Configuration

```
HW201 Sensor → Arduino Uno

OUT pin  → Pin 6 (Digital Input)
VCC      → 5V (or 3.3V)
GND      → GND
```

### Connection Diagram

```
┌─────────────────────┐
│     HW201 Sensor    │
├─────────────────────┤
│ OUT ────────→ Pin 6 │ (Digital Input)
│ VCC ────────→ 5V    │
│ GND ────────→ GND   │
└─────────────────────┘
```

---

## 💻 Command Reference

### New Command: OBSTACLE_CHECK

**Purpose**: Read the current state of the HW201 obstacle sensor

**Command Format**:
```
OBSTACLE_CHECK
```

**Response**:
```
[HW201] Obstacle sensor: OBSTACLE DETECTED!
OBSTACLE_STATE=0

OR

[HW201] Obstacle sensor: CLEAR
OBSTACLE_STATE=1
```

**Return Values**:
- `0` = Obstacle detected (OUT pin LOW)
- `1` = No obstacle / Clear (OUT pin HIGH)

---

## 🎮 WinUI Control Interface

### New Sensor Panel

**Location**: Arduino Control → Sensor Reading Panel

**HW201 Obstacle Sensor (Pin 6)**
- Display: Real-time obstacle status
- Status: "CLEAR" or "OBSTACLE DETECTED"
- Button: "Read" - Updates sensor status

```
┌─────────────────────────────────────┐
│ HW201 Obstacle (Pin 6)              │
├─────────────────────────────────────┤
│  ┌─────────────────────────────────┐│
│  │ CLEAR / OBSTACLE DETECTED       ││  ← Real-time display
│  └─────────────────────────────────┘│
│  [Read]                             │  ← Click to update
└─────────────────────────────────────┘
```

---

## 📊 System Integration

### Automatic Detection

The system automatically monitors the HW201 sensor:

```
Polling Interval: 1 second (configurable)
Response Time:    <100ms
Display Format:   Real-time card
Update Trigger:   Button click or auto-polling
```

### System State Tracking

The obstacle detection state is tracked in the system:

```
systemState.obstacleDetected = (boolean)
  true  = Obstacle detected
  false = Path clear
```

### JSON State Export

Get complete system state including HW201 status:

**Command**:
```
GET_STATE
```

**Response**:
```json
STATE={
  "motorSpeed":0,
  "motorDir":1,
  "motorEnabled":false,
  "limitTriggered":false,
  "obstacleDetected":false,
  "pressure":512,
  "uptime":3600
}
```

---

## 🔧 Usage Examples

### Example 1: Basic Obstacle Detection

**Check if path is clear**:
```
1. WinUI App → Arduino Control
2. Sensor Reading Panel
3. Click "Read" under HW201
4. Status updates: "CLEAR" or "OBSTACLE DETECTED"
```

**Or via Serial**:
```
Command:  OBSTACLE_CHECK
Response: [HW201] Obstacle sensor: CLEAR
		  OBSTACLE_STATE=1
```

### Example 2: Automated Obstacle Avoidance

**Safe motor operation with obstacle detection**:
```
1. Motor Enable: MOTOR_ENABLE=1
2. Check Obstacle: OBSTACLE_CHECK
3. If clear (state=1):
   - Set direction: MOTOR_DIR=1
   - Set speed: MOTOR_SPEED=150
   - Start motor
4. Continuously monitor: OBSTACLE_CHECK
5. If obstacle (state=0):
   - Stop: MOTOR_STOP
6. Resume when clear
```

### Example 3: Combined Sensor Monitoring

**Check all sensors including HW201**:
```
// Read all sensors at once
LIMIT_CHECK     → Get limit switch state
OBSTACLE_CHECK  → Get obstacle status
PRESSURE_READ   → Get pressure value
STATUS          → Get all system status
```

---

## 📈 Real-Time Monitoring

### Sensor Monitoring Display

The WinUI app shows:
```
┌─ SENSOR READING ────────────────────────┐
│                                         │
│  Limit Switch: [OPEN]  [Read]         │
│  HW201 Obstacle: [CLEAR]  [Read]      │
│  Pressure: [512]  [Read]              │
│  Analog: Pin [0]  [---]  [Read]       │
│                                         │
└─────────────────────────────────────────┘
```

### Automatic Updates

Sensors are automatically polled every 1 second:
- Limit Switch status
- HW201 Obstacle detection
- Pressure reading
- System uptime

---

## ⚙️ Technical Specifications

### HW201 Sensor Specifications

| Parameter | Value |
|-----------|-------|
| Detection Range | 2-40 cm |
| Operating Voltage | 3.3V - 5V |
| Output Type | Digital (HIGH/LOW) |
| Response Time | <100ms |
| Current Draw | ~20mA |
| Output Level | HIGH (clear), LOW (obstacle) |
| Detection Angle | ~35 degrees |

### Arduino Integration

| Setting | Value |
|---------|-------|
| Pin | 6 (Digital Input) |
| Pin Mode | INPUT (no pull-up) |
| Sample Rate | 1 Hz |
| Buffer Size | Integrated |
| Priority | Real-time |

---

## 🔍 Sensor States

### State Interpretation

```
Sensor Output     Display        Status
─────────────────────────────────────────
HIGH (1)      → CLEAR         Path is clear
LOW (0)       → OBSTACLE      Object detected

Time to detect: <100ms
Time to clear:  <100ms
```

### LED Visual Reference (if present)

Some HW201 modules have an LED indicator:
- **Red LED ON**: Obstacle detected
- **Red LED OFF**: Path clear

---

## 🛠️ Troubleshooting

### Issue: Sensor Always Shows "OBSTACLE DETECTED"

**Possible Causes**:
1. Object too close to sensor
2. Infrared interference from sunlight
3. Wrong detection range
4. Sensor misaligned

**Solutions**:
- Move obstacles away (>40cm)
- Block sunlight/bright light
- Check lens cleanliness
- Verify sensor orientation (lens pointing forward)

### Issue: Sensor Never Detects Obstacles

**Possible Causes**:
1. Sensor not powered
2. Wrong pin configuration
3. Object material doesn't reflect IR
4. Detection range too far

**Solutions**:
- Verify 5V power to sensor
- Check pin 6 connection
- Test with reflective object (white paper)
- Position object within 2-40cm range

### Issue: False Detections

**Possible Causes**:
1. Bright infrared light (sunlight)
2. Reflective surfaces nearby
3. Electrical noise

**Solutions**:
- Shield sensor from direct sunlight
- Remove reflective objects
- Add 100nF capacitor across power pins

---

## 🎯 Common Patterns

### Pattern 1: Obstacle Warning

```
Loop every 100ms:
  1. Read obstacle: OBSTACLE_CHECK
  2. If detected:
	 - Log warning
	 - Sound alarm
	 - Stop motor
  3. If clear:
	 - Resume normal operation
```

### Pattern 2: Safe Navigation

```
Before each move:
  1. Check obstacle: OBSTACLE_CHECK
  2. If clear (state=1):
	 - Execute movement
	 - Monitor during movement
	 - Check again if moving far
  3. If blocked (state=0):
	 - Wait
	 - Retry after delay
```

### Pattern 3: Safety Interlock

```
Motor safety sequence:
  1. Motor enable
  2. Check obstacle BEFORE moving
  3. If obstacle detected:
	 - Reject motion command
	 - Alert user
	 - Keep motor disabled
  4. If clear:
	 - Allow motion
	 - Continue monitoring
```

---

## 📝 Status Display

### Serial Monitor Output

```
[HW201] Obstacle sensor: CLEAR
OBSTACLE_STATE=1

[HW201] Obstacle sensor: OBSTACLE DETECTED!
OBSTACLE_STATE=0
```

### WinUI Dashboard Display

```
HW201 Obstacle (Pin 6)
┌────────────────────┐
│      CLEAR         │  ← Green text
│                    │
│  [Read]            │
└────────────────────┘

OR

HW201 Obstacle (Pin 6)
┌────────────────────┐
│ OBSTACLE DETECTED  │  ← Red/Warning
│                    │
│  [Read]            │
└────────────────────┘
```

---

## 🔄 System Commands with HW201

### Get Full Status

```
Command: STATUS
Response:
  ║ Limit Switch: OPEN                     ║
  ║ Obstacle Detected: NO                  ║
  ║ Pressure: 512                          ║
  ║ Uptime: 3600 seconds                   ║
```

### Get JSON State

```
Command: GET_STATE
Response:
  STATE={
	"limitTriggered":false,
	"obstacleDetected":false,
	...
  }
```

### Sensor Scan

```
Command: SENSOR_SCAN
Response:
  [SCAN] Limit Switch: FOUND
  [SCAN] HW201 Obstacle: FOUND
  [SCAN] Analog sensors: Found
```

---

## ✅ Verification Checklist

### Setup Verification

- [ ] HW201 sensor connected to Pin 6
- [ ] VCC connected to 5V
- [ ] GND connected to GND
- [ ] Arduino responds to OBSTACLE_CHECK
- [ ] WinUI shows sensor panel
- [ ] Sensor detects test object

### Functionality Verification

- [ ] Sensor reads "CLEAR" when path is open
- [ ] Sensor reads "OBSTACLE" when object present
- [ ] Detection range is 2-40cm
- [ ] Response time <100ms
- [ ] WinUI updates in real-time
- [ ] JSON state includes obstacle status

---

## 📚 Complete Command List

### Updated Commands Reference

```
MOTOR_SPEED=0-255      Motor speed
MOTOR_DIR=0|1          Motor direction
MOTOR_ENABLE=0|1       Motor enable/disable
MOTOR_STOP             Emergency stop

STEP_PULSE=count       Stepper pulses
STEP_SPEED=Hz          Stepper frequency
HOME                   Move to home

LIMIT_CHECK            Limit switch
OBSTACLE_CHECK         HW201 sensor (NEW)
PRESSURE_READ          Pressure sensor
ANALOG_READ=pin        Analog input
DIGITAL_READ=pin       Digital input

DIGITAL_WRITE=pin,val  Digital output
PWM_WRITE=pin,val      PWM output

STATUS                 System status
GET_STATE              JSON state
SENSOR_SCAN            Find sensors
UPTIME                 System uptime
RESET                  Reset system
HELP                   Show commands
PING                   Echo test
```

---

## 🎓 Learning Resources

### Quick Reference

| What | How |
|------|-----|
| Read sensor | Click "Read" button or send `OBSTACLE_CHECK` |
| Check status | View WinUI panel or send `STATUS` |
| Get all data | Send `GET_STATE` (returns JSON) |
| Find sensors | Send `SENSOR_SCAN` |

### Common Uses

1. **Collision Prevention** - Stop motor if obstacle detected
2. **Safe Zone Monitoring** - Warn when object enters zone
3. **Autonomous Navigation** - Navigate around obstacles
4. **Security System** - Detect unauthorized intrusion
5. **Access Control** - Trigger alarm on detection

---

```
╔════════════════════════════════════════════════════════════════╗
║                                                                ║
║     HW201 INFRARED OBSTACLE DETECTION - FULLY INTEGRATED      ║
║                                                                ║
║  ✓ Command: OBSTACLE_CHECK                                   ║
║  ✓ Pin: 6 (Digital Input)                                    ║
║  ✓ Range: 2-40 cm                                            ║
║  ✓ WinUI: Sensor Reading Panel                               ║
║  ✓ Status: Automatic monitoring every 1 second               ║
║  ✓ JSON: Included in GET_STATE response                      ║
║                                                                ║
║  USAGE:                                                        ║
║  1. WinUI App → Arduino Control → Sensor Reading              ║
║  2. Click "Read" under HW201 Obstacle                         ║
║  3. See real-time status: "CLEAR" or "OBSTACLE DETECTED"      ║
║                                                                ║
║  OR send command:                                              ║
║  OBSTACLE_CHECK                                                ║
║                                                                ║
║  Response: OBSTACLE_STATE=0|1                                 ║
║                                                                ║
║  Status: ✅ FULLY INTEGRATED & READY                         ║
║                                                                ║
╚════════════════════════════════════════════════════════════════╝
```

---

**Version**: 1.0  
**Status**: ✅ Production Ready  
**Date**: December 2024  
**Sensor**: HW201 Infrared Obstacle Detection  
**Arduino Pin**: 6  
**Command**: OBSTACLE_CHECK
