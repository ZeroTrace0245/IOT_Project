# TMC2209 Driver Identification - Visual Checklist

## 🔌 Wiring Verification Diagram

```
ARDUINO UNO                           TMC2209 DRIVER
┌─────────────────┐                  ┌──────────────────┐
│ GND ────────────┼──────────────────┼─ GND (Black pin) │
│ 5V              │                  │                  │
│ 3 (STEP) ───────┼──────────────────┼─ STEP (Left)     │
│ 4 (DIR) ────────┼──────────────────┼─ DIR (Left)      │
│ 5 (EN) ─────────┼──────────────────┼─ EN (Left)       │
│ 2 (LIMIT) ──────┼──────────────────┼─ LIMIT_SWITCH    │
│ A0 (PRESSURE) ──┼──────────────────┼─ PRESSURE_SENSOR │
└─────────────────┘                  └──────────────────┘
											│
						NEMA 17 MOTOR       │
					┌─────────────┐         │
					│ Black (1A) ──┼─────────┤
					│ Green (1B) ──┼─────────┤ 1A─1B
					│ Red (2A) ────┼─────────┤ 2A─2B
					│ Blue (2B) ───┼─────────┤
					└─────────────┘         │
											│
EXTERNAL POWER:
	12V ─────────────────────────────┬─ VM (Red pin)
	GND ──────────────┬──────────────┼─ GND
					  │              │
					  └──C+ (100µF)  │
						 │           │
					  C– ┴───────────┴─ GND
```

## 🧪 Physical Pin Locations

### Arduino Uno Pins (Top View)
```
Front of Arduino (with USB port at bottom)

Row 1:  [ ] [ ] [ ] [ ] [ ] [ ] [ ]  (empty slots)

Row 2:  IOREF, GND, 2, 4, 6, 8, 10, 12, GND
		(right side pins)

Row 3:  5V, RST, 1, 3, 5, 7, 9, 11, GND
		(left side pins)

Row 4:  A0  A1  A2  A3  A4  A5  (analog pins, bottom)
```

**Our Connections:**
```
Pin 2   → LIMIT_SWITCH (Row 2, 3rd from left)
Pin 3   → STEP         (Row 3, 4th from left)
Pin 4   → DIR          (Row 2, 4th from left)
Pin 5   → EN           (Row 3, 5th from left)
A0      → PRESSURE     (Bottom, first analog pin)
GND     → GND          (Any GND pin)
```

## 📍 TMC2209 Module Pin Labels

### Left Side (Black Pins - Signal/Control)
```
Looking at driver from above:
Top → [ VIO ]
	  [ GND ]
	  [ STEP]  ← Pin 3
	  [ DIR ]  ← Pin 4
	  [ EN  ]  ← Pin 5
	  [ (reserved) ]
Bottom
```

### Right Side (Red Pins - Motor Output)
```
Looking at driver from above:
Top → [ 1A  ]  ← Motor Black wire
	  [ 1B  ]  ← Motor Green wire
	  [ 2A  ]  ← Motor Red wire
	  [ 2B  ]  ← Motor Blue wire
	  [ GND ]
Bottom
```

### Bottom Side (Power)
```
Looking at driver from front:
Left → [ GND (Black) ]
	  [ VM  (Red)   ]  ← 12V supply
Right
```

## ✅ Connection Verification Checklist

### Power Section
```
□ 12V power supply positive → VM pin (red label)
□ 12V power supply negative → GND pin (black label)  
□ 100µF capacitor installed:
  □ Positive (longer leg) → VM
  □ Negative (shorter leg) → GND
□ Capacitor physically close to VM/GND pins
□ No shorts between power pins
```

### Motor Section
```
□ Black wire (Coil 1A) → 1A pin
□ Green wire (Coil 1B) → 1B pin
□ Red wire (Coil 2A) → 2A pin
□ Blue wire (Coil 2B) → 2B pin
□ All 4 motor wires firmly seated
□ No loose or exposed wires
```

### Signal Section
```
□ Arduino Pin 3 → STEP pin
□ Arduino Pin 4 → DIR pin
□ Arduino Pin 5 → EN pin
□ Arduino GND → GND pin (any GND on driver)
```

### Sensor Section
```
□ Limit Switch → Arduino Pin 2
  □ Limit switch has 3 wires:
	□ Red (power) → 5V on Arduino
	□ Black (ground) → GND
	□ Yellow (signal) → Pin 2

□ Pressure Sensor → Arduino A0
  □ Red (power) → 5V
  □ Black (ground) → GND
  □ Yellow/Green (signal) → A0
```

## 🚨 Common Wiring Mistakes (DON'T DO THESE!)

### ❌ Mistake 1: Motor Wires Crossed
```
WRONG:                  RIGHT:
1A ← Red               1A ← Black
1B ← Blue              1B ← Green
2A ← Black             2A ← Red
2B ← Green             2B ← Blue
Result: Motor vibrates only ✗
```

### ❌ Mistake 2: Missing Capacitor
```
Result: 
- Driver overheats
- Voltage spikes damage components
- Motor jerks or fails
✓ ALWAYS install 100µF capacitor
```

### ❌ Mistake 3: Wrong Baud Rate
```
WRONG: 115200 baud (ESP32 speed)
RIGHT: 9600 baud (Arduino Uno standard)
```

### ❌ Mistake 4: Swapped Power
```
WRONG:
GND → VM
12V → GND
Result: Destroys driver immediately! 💥
```

### ❌ Mistake 5: Missing Ground Connection
```
Result: Voltage floating, unpredictable behavior
✓ Always connect Arduino GND to driver GND
```

## 📊 Test Values to Expect

After uploading DriverDiagnostics.ino, Serial Monitor should show:

### Test 1 - Motor Enable
```
ENABLE_PIN set to HIGH (motor DISABLED)     ← Manual check
ENABLE_PIN set to LOW (motor ENABLED)       ← Green LED should be ON
Check: Motor should be harder to turn       ← Try rotating shaft
```

### Test 2 - Direction
```
DIR_PIN set to HIGH (FORWARD direction)
DIR_PIN set to LOW (REVERSE direction)
Check: Motor should reverse smoothly
```

### Test 3 - Step Signal
```
Sending 50 step pulses (forward)...
50 steps sent
Check: Motor shaft rotated ~1/4 turn
Sending 50 step pulses (reverse)...
50 reverse steps sent
Check: Motor returned to start position
```

### Test 4 - Limit Switch
```
Limit Switch State: HIGH (NOT TRIGGERED - expected at rest)
Check: Press the limit switch and it should read LOW
State changed to: LOW    ← Appears when you press switch
```

### Test 5 - Pressure Sensor
```
Reading 1: RAW=42 (0-1023) | Voltage=0.21V
Reading 2: RAW=45 (0-1023) | Voltage=0.22V
Reading 3: RAW=48 (0-1023) | Voltage=0.23V
(Values should be fairly consistent without pressure)
```

### Test 6 - Full Motor Movement
```
Executing 200 steps (1 full revolution)...
→ FORWARD: 200 steps
✓ Completed
← REVERSE: 200 steps
✓ Completed
Motor test complete!
```

## 🎯 Quick Verification Procedure

1. **Visual Check** (30 seconds)
   - Power LED on driver: ✓
   - All wires connected: ✓
   - Capacitor installed: ✓

2. **Upload Test** (2 minutes)
   - Arduino IDE → Upload DriverDiagnostics.ino
   - Wait for "Done uploading" message

3. **Serial Monitor** (30 seconds)
   - Open Serial Monitor (9600 baud)
   - Watch automatic tests run
   - Read results

4. **Verification** (Instant)
   - All tests show ✓ → Driver working! 🎉
   - Any test shows ✗ → Check wiring above

## 📞 If Verification Fails

1. **No Serial Output**
   - Check COM port is correct
   - Check 9600 baud is selected
   - Re-upload sketch

2. **Power Issues (No Green LED)**
   - Check 12V power is plugged in
   - Measure voltage with multimeter
   - Verify GND connection

3. **Motor Problems**
   - Check motor wires: Black, Green, Red, Blue
   - Verify each wire is in correct pin (1A, 1B, 2A, 2B)
   - Try swapping RED ↔ BLUE
   - Try swapping BLACK ↔ GREEN

4. **Sensor Issues**
   - Check limit switch has 3 wires
   - Check pressure sensor wiring
   - Try reading sensor values

---

**After verification passes, you're ready to upload FootScanner.ino! 🚀**
