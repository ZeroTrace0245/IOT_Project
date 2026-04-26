# ⚡ MOTOR TESTING GUIDE - PIN 8 (PWM)

## Overview
This guide explains how to test your motor connected to **Pin 8** using PWM (Pulse Width Modulation) speed control.

---

## Part 1: Hardware Setup

### Wiring Diagram
```
Motor
├─ Signal → Arduino Pin 8 (PWM capable)
└─ Ground → Arduino GND
```

### Pin 8 Specifications
- **Pin Type**: PWM (Pulse Width Modulation)
- **Speed Range**: 0-255 (0% to 100%)
- **Frequency**: 490 Hz (standard Arduino PWM)
- **Min Speed**: 0 (motor off)
- **Max Speed**: 255 (full power)

### Verification Checklist
- [ ] Motor connected to Pin 8
- [ ] Ground wire connected to Arduino GND
- [ ] Arduino connected via USB
- [ ] WinUI application running
- [ ] Serial port detected in application

---

## Part 2: Quick Test (5 Minutes)

### Option 1: WinUI Speed Test (Test 8)

1. **Launch the WinUI App**
   - Open Smart Footwear Dashboard
   - Navigate to **Driver Testing** tab

2. **Select Test 8: Motor Speed Test**
   - From dropdown, select `8. Motor Speed Test (Pin 8)`
   - Click **▶ Run Test**

3. **Expected Output**
   ```
   Test 8: Motor Speed Test (Pin 8)
   Testing PWM motor speed control...

   Step 1: Ramping speed from 0% to 100%
	 Speed 0%
	 Speed 10%
	 Speed 20%
	 ... (up to 100%)

   Step 2: Ramping speed from 100% to 0%
	 Speed 100%
	 Speed 90%
	 ... (down to 0%)

   ✓ Motor speed test complete!
   ✓ You should have heard gradual speed increase
   ✓ Check vibration increased with speed
   ```

4. **What to Listen For**
   - Initial quiet hum at low speeds
   - Gradually increasing pitch as speed increases
   - Vibration increasing
   - Return to quiet at lower speeds

---

## Part 3: Real-Time Monitoring (Professional)

### Option 2: WinUI Speed Monitor (Test 9) ⭐ **RECOMMENDED**

This is the best way to control and verify motor operation.

#### Step 1: Access Motor Monitor

1. Go to **Driver Testing** tab (continue scrolling down)
2. Find **"⚡ Motor Speed Monitor - Pin 8 (PWM)"** section
3. You'll see a slider and quick preset buttons

#### Step 2: Test Using Slider

1. **Drag the slider** from left (0%) to right (100%)
2. You should hear motor speed change smoothly
3. **Click "Apply Speed"** to send current slider value to Arduino
4. Motor should respond immediately

#### Step 3: Test Using Presets

Quick preset buttons (easier than slider):
- **0% (Off)** - Stops motor completely
- **25%** - Motor at quarter speed
- **50%** - Motor at half speed  
- **75%** - Motor at three-quarter speed
- **100% (Max)** - Full power (red button)

**Usage**:
```
1. Click "25%" button → Motor at 25% speed
2. Wait 2 seconds
3. Click "50%" button → Motor at 50% speed
4. Wait 2 seconds
5. Click "100%" button → Motor at full speed
6. Click "0%" button → Motor stops
```

#### Step 4: Interpret Feedback

**Activity Log Shows**:
- ⏹️ Stopped - Motor speed set to 0
- 🐢 Slow - Motor speed 0-33%
- 🚗 Medium - Motor speed 33-66%
- 🚀 Fast - Motor speed 66-100%

**Status Display Shows**:
- Current speed value (0-255)
- Percentage (0-100%)
- Visual indicator (icon + text)

#### Step 5: Verify Results

| Check | Expected | Status |
|-------|----------|--------|
| 0% speed | Motor silent | ✓ |
| 25% speed | Quiet, slow | ✓ |
| 50% speed | Moderate sound | ✓ |
| 75% speed | Loud, fast | ✓ |
| 100% speed | Loudest, fastest | ✓ |
| Smooth transition | No jumps/stutters | ✓ |
| Instant response | No delay | ✓ |

---

## Part 4: Arduino IDE Method

**File**: `Arduino/MotorTest_Pin8.ino`

### Test Sequence

1. **Speed Ramp Up** (0→100%)
   - Gradual increase in speed
   - 25% increments every 0.3 seconds

2. **Speed Ramp Down** (100→0%)
   - Gradual decrease in speed
   - 25% decrements every 0.3 seconds

3. **Fixed Speeds**
   - Tests specific speeds: 50, 100, 150, 200, 255
   - Each speed held for 0.8 seconds

4. **Pulse Test**
   - Rapid ON/OFF cycles
   - 5 pulses at speed 200
   - 0.5 second on, 0.5 second off

### How to Use
1. Upload sketch to Arduino
2. Open Serial Monitor (9600 baud)
3. Watch motor spin and check output
4. Verify speeds match expected values

---

## Part 5: Understanding PWM Speeds

### Speed Values Explained

```
0 = 0%     → Motor OFF (no power)
63 = 25%   → Very slow spin
127 = 50%  → Medium speed
191 = 75%  → Fast spin
255 = 100% → Maximum power
```

### Sound/Vibration Progression

```
Speed 0-50:    Silent or barely audible
Speed 50-100:  Quiet hum
Speed 100-150: Noticeable sound
Speed 150-200: Loud, significant vibration
Speed 200-255: Very loud, strong vibration
```

### Power Consumption

```
Speed 0-50:    Minimal power
Speed 50-100:  Low power
Speed 100-150: Medium power
Speed 150-200: High power
Speed 200-255: Maximum power (most heat)
```

---

## Part 6: Troubleshooting

### Problem: Motor doesn't spin

**Cause**: No power or bad connection

**Fix**:
1. Check wires at both ends
2. Verify Arduino connection
3. Try Serial Monitor first to see if values sent
4. Check motor for mechanical issues

### Problem: Motor spins at same speed regardless

**Cause**: PWM not working, motor locked

**Fix**:
1. Verify Pin 8 is PWM capable
2. Check firmware is sending correct values
3. Try different speeds (0, 127, 255)
4. Motor may be stalled - remove mechanical load

### Problem: Motor stutters or jerks

**Cause**: Insufficient power or voltage drop

**Fix**:
1. Check power supply voltage (should be stable)
2. Reduce load on motor
3. Check wiring for loose connections
4. Use separate power supply if needed

### Problem: No sound but serial shows values

**Cause**: Motor issue or wiring

**Fix**:
1. Check motor polarity
2. Verify mechanical freedom (not blocked)
3. Test motor with direct 5V connection
4. Use multimeter to verify voltage at Pin 8

### Problem: WinUI shows values but motor doesn't respond

**Cause**: Serial communication or Arduino issue

**Fix**:
1. Verify COM port correct
2. Check Arduino firmware handles MOTOR_SPEED command
3. Test with Arduino IDE first
4. Check Arduino for error messages

---

## Part 7: Success Criteria ✓

Your motor is **working correctly** if:

✅ Motor spins at 0% (off) - completely silent

✅ Motor spins at 25% - very slow, barely audible

✅ Motor spins at 50% - moderate speed, clear sound

✅ Motor spins at 75% - fast, loud vibration

✅ Motor spins at 100% - maximum speed and power

✅ Speed changes smoothly (no jumps)

✅ Response is instant (< 100ms)

✅ Sound increases with speed

✅ Vibration increases with speed

✅ No stuttering or stalling

---

## Part 8: Serial Commands

### From Serial Console

```
Command Format:          Result:
MOTOR_SPEED=0           Motor OFF
MOTOR_SPEED=63          Motor at 25%
MOTOR_SPEED=127         Motor at 50%
MOTOR_SPEED=191         Motor at 75%
MOTOR_SPEED=255         Motor at 100%
```

### Expected Response
Arduino will accept and apply the command. Motor speed will change immediately.

---

## Part 9: Technical Details

### PWM Frequency
- **490 Hz** - Standard Arduino frequency
- Not heard by humans (ultrasonic)
- Creates variable speed through duty cycle

### Duty Cycle
```
Speed 0:    0% duty cycle (always off)
Speed 127:  50% duty cycle (half on/off)
Speed 255:  100% duty cycle (always on)
```

### Power Calculation
```
Power = (Speed / 255) × Maximum_Power

Speed 127 = 50% of maximum power
Speed 255 = 100% of maximum power
```

---

## Part 10: Complete Test Workflow

### Full Verification Procedure

```
1. SETUP (2 minutes)
   □ Motor wired to Pin 8 + GND
   □ Arduino connected via USB
   □ WinUI app running
   □ COM port selected
   □ Connected (green indicator)

2. QUICK TEST (5 minutes)
   □ Select "Test 8: Motor Speed Test"
   □ Click "Run Test"
   □ Listen to gradual speed increase then decrease
   □ Verify all speeds audible

3. DETAILED TEST (10 minutes)
   □ Find "Motor Speed Monitor" panel
   □ Click preset buttons: 0%, 25%, 50%, 75%, 100%
   □ Listen to each speed for 2 seconds
   □ Check activity log for all commands
   □ Verify smooth speed transitions

4. VALIDATION (2 minutes)
   □ All speeds heard? ✓
   □ Sound increases with speed? ✓
   □ Instant response? ✓
   □ No stuttering? ✓

TOTAL TIME: ~20 minutes
```

---

## Part 11: Quick Reference

| Task | Location | Action |
|------|----------|--------|
| Quick Test | Driver Testing → Test 8 | Click "Run Test" |
| Real-Time Control | Driver Testing → Motor Monitor | Use slider or buttons |
| Arduino Method | Arduino/MotorTest_Pin8.ino | Upload & monitor |
| Send Command | Serial Console | MOTOR_SPEED=value |
| Clear Log | Motor Monitor | Click activity log button |
| Stop Motor | Motor Monitor | Click "Stop Motor" button |

---

## Part 12: Safety Notes

⚠️ **Important Safety Information**:

- Don't block motor while running
- Keep hands away from moving parts
- Unplug motor if it overheats
- Don't exceed 255 speed value
- Motor will get warm at high speeds
- Use protective equipment if needed

---

## Success Example

### Correct Test Results
```
Starting Motor Test:

[14:32:15] ▶ Motor testing started
[14:32:16] 🐢 Speed: 50/255 (20%)   ← Very quiet
[14:32:20] 🚗 Speed: 127/255 (50%)  ← Moderate sound
[14:32:24] 🚀 Speed: 255/255 (100%) ← Full power
[14:32:28] ⏹️ Speed: 0/255 (0%)      ← Stopped

Result: ✅ Motor working perfectly
```

---

## Need Help?

### Common Questions

**Q: What's a normal PWM frequency?**  
A: 490 Hz is standard for Arduino Pin 8

**Q: Can I go above 255?**  
A: No, 255 is the maximum

**Q: Why does 50% not sound like half speed?**  
A: Motor behavior is nonlinear; may need more power

**Q: Should motor be hot?**  
A: Warm is normal at high speed; very hot means issue

**Q: Why no sound?**  
A: Check wiring, power supply, motor connection

---

**Last Updated**: December 2024  
**Firmware Version**: Motor Test v1.0  
**Compatible Boards**: Arduino Uno (Pin 8 PWM)  
**Test Interface**: WinUI Smart Footwear Dashboard
