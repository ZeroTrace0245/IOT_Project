# TMC2209 Driver Identification & Testing Guide

## How to Verify If Your TMC2209 Driver Is Properly Connected

### Option 1: Use the Diagnostic Sketch (RECOMMENDED)

We've created `Arduino/DriverDiagnostics.ino` - a comprehensive test that will identify if your driver is working.

**Steps:**
1. Upload `DriverDiagnostics.ino` to your Arduino Uno
2. Open Arduino IDE Serial Monitor (9600 baud)
3. Watch the tests run and follow the on-screen instructions

**What Each Test Checks:**

#### Test 1: Motor Enable/Disable
- Sets ENABLE_PIN HIGH (disabled) and LOW (enabled)
- **Expected**: Motor becomes hard to turn when enabled
- **Success Indicator**: Green LED on TMC2209 lights up when enabled

#### Test 2: Direction Control
- Toggles DIR_PIN between HIGH and LOW
- **Expected**: Motor switches between forward and reverse directions
- **Success Indicator**: Motor shaft can reverse smoothly

#### Test 3: Step Signal
- Sends 50 step pulses forward, then 50 backward
- **Expected**: Motor rotates slightly forward and back to original position
- **Success Indicator**: ~0.25 revolution of motor shaft (50 steps ÷ 200 steps/rev)

#### Test 4: Limit Switch
- Reads the limit switch state
- **Expected**: HIGH (not triggered) at rest, LOW when pressed
- **Success Indicator**: Switch state changes when you press the limit switch

#### Test 5: Pressure Sensor
- Reads analog pressure sensor value (A0)
- **Expected**: Reading between 0-1023
- **Success Indicator**: Changes when you apply pressure to sensor

#### Test 6: Full Motor Movement
- Complete 1 full revolution forward and backward
- **Expected**: Motor completes 360° rotation smoothly
- **Success Indicator**: Motor returns to starting position

---

### Option 2: Quick Visual Inspection

Before running tests, visually check:

```
✓ Green LED on TMC2209 board
  - Should light up when motor is enabled

✓ Motor connections (4 wires):
  Black → 1A
  Green → 1B
  Red   → 2A
  Blue  → 2B

✓ Pin connections:
  Arduino Pin 3 → STEP
  Arduino Pin 4 → DIR
  Arduino Pin 5 → EN
  Arduino Pin 2 → LIMIT_SWITCH
  Arduino A0   → PRESSURE_SENSOR

✓ Power connections:
  12V → VM
  GND → GND
  100µF Capacitor: + to VM, - to GND (CRITICAL!)
```

---

### Option 3: Manual Serial Commands (From WinUI App)

Once connected via Serial (9600 baud), send commands:

```
PING                              → Should get: PONG=OK
TEST                              → Motor does forward/reverse/forward test
READ                              → Reads pressure sensor
HOME                              → Moves to limit switch
MEASURE                           → Starts continuous measurement
```

---

### What Could Go Wrong & How to Fix

| Problem | Diagnosis | Fix |
|---------|-----------|-----|
| Motor doesn't move | Check EN pin / power | Verify 12V supply, check enable pin high/low |
| Motor vibrates but doesn't spin | Wrong coil wiring | Swap RED ↔ BLUE or BLACK ↔ GREEN |
| Motor locks/won't move at all | Motor disabled or stalled | Enable motor (EN=LOW), reduce STEP_DELAY_US |
| No serial communication | COM port or baud rate | Check 9600 baud, verify USB driver |
| Limit switch not detected | Switch wiring or pin | Verify pin 2 to switch, check pullup |
| Pressure sensor reads 0 or 1023 | Sensor not connected | Check A0 wiring, verify analog input |
| Green LED doesn't light on driver | Power or wiring | Check 12V supply, verify connections |
| Arduino doesn't respond to commands | Code issue | Re-upload DriverDiagnostics.ino or FootScanner.ino |

---

### Expected Serial Output

When you run DriverDiagnostics.ino, you should see:

```
========================================
TMC2209 DRIVER DIAGNOSTICS
========================================

Pin Configuration: OK
STEP_PIN: Pin 3
DIR_PIN: Pin 4
ENABLE_PIN: Pin 5
LIMIT_SWITCH_PIN: Pin 2
PRESSURE_SENSOR_PIN: A0

========================================
TEST CYCLE
========================================

[TEST 1] Motor Enable/Disable Detection
-----------------------------------------
ENABLE_PIN set to HIGH (motor DISABLED)
ENABLE_PIN set to LOW (motor ENABLED)
✓ Check: Motor should be harder to turn now (if NEMA 17 is connected)
✓ Indicator: Green LED on driver should light up

[TEST 2] Direction Control
-----------------------------------------
DIR_PIN set to HIGH (FORWARD direction)
DIR_PIN set to LOW (REVERSE direction)
✓ Check: Motor should be able to switch directions

[TEST 3] Step Signal / Motor Movement
-----------------------------------------
Sending 50 step pulses (forward)...
✓ 50 steps sent
✓ Check: Motor shaft should have rotated slightly
...
```

---

### Hardware Checklist

Before testing, verify:

- [ ] Arduino Uno connected to USB
- [ ] TMC2209 driver board powered (12V supply)
- [ ] 100µF capacitor installed on VM and GND
- [ ] All 5 pins connected from Arduino to driver:
  - [ ] Pin 3 → STEP
  - [ ] Pin 4 → DIR
  - [ ] Pin 5 → EN
  - [ ] Pin 2 → LIMIT_SWITCH
  - [ ] A0 → PRESSURE_SENSOR
- [ ] NEMA 17 motor connected (4 wires to 1A, 1B, 2A, 2B)
- [ ] Limit switch connected to Pin 2
- [ ] Pressure sensor connected to A0

---

### Success Criteria

Your driver is properly identified if:

✅ Green LED lights when EN=LOW  
✅ Motor steps forward/backward correctly  
✅ Limit switch detects presses  
✅ Pressure sensor reads changing values  
✅ Serial commands are recognized  

**If all 5 checks pass → Your driver is working!** 🎉

---

### Next Steps

Once verified:
1. Upload `Arduino/FootScanner.ino` (main firmware)
2. Test with WinUI dashboard
3. Run measurement calibration
4. Deploy to production

---

## Support

If tests fail:
1. Check wiring connections (take a photo)
2. Verify 12V power supply is connected
3. Confirm 100µF capacitor is installed
4. Try swapping motor coil pairs if motor doesn't move
5. Review Arduino IDE serial output for error messages
