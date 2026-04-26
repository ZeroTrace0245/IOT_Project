# ⚙️ STEP SIGNAL TESTING GUIDE - PIN 3

## Overview
This guide explains how to test **stepper motor step pulses** on **Pin 3** for precise motor control.

---

## Part 1: Hardware Setup

### Wiring Diagram
```
Stepper Motor Driver
├─ STEP signal → Arduino Pin 3
├─ Direction signal → (separate pin)
└─ Ground → Arduino GND
```

### Pin 3 Specifications
- **Pin Type**: Digital Output (PWM capable but used as digital)
- **Pulse Width**: 2 microseconds (standard for stepper drivers)
- **Pulse Type**: Square wave (HIGH/LOW)
- **Step Range**: 1-200+ steps per command
- **Frequency**: Adjustable (1 Hz to 100+ Hz)

### Verification Checklist
- [ ] Stepper motor driver connected to Pin 3
- [ ] Ground wire connected to Arduino GND
- [ ] Arduino connected via USB
- [ ] WinUI application running
- [ ] Serial port detected in application

---

## Part 2: Understanding Step Signals

### What is a Step Signal?

A step signal is a series of **short pulses** (2 microseconds) that tell a stepper motor driver to move the motor one step.

```
Pulse Timing:
┌─────────────┐
│ HIGH (2µs)  │                    ┌─────────────┐
│             │                    │ HIGH (2µs)  │
└─────────────┴────────────────────┴─────────────┴────...
  ▲           ▲                    ▲
  │           └── Delay (5-100ms)  │
  │                                └── Next pulse
  Start pulse
```

### How Steppers Work

```
Number of Steps → Motor Rotation Angle

1 step = 1.8° (for 200-step motors)
50 steps = 90° (quarter turn)
100 steps = 180° (half turn)
200 steps = 360° (full rotation)
```

---

## Part 3: Quick Test (5 Minutes)

### Option 1: WinUI Step Sequences

1. **Launch the WinUI App**
   - Open Smart Footwear Dashboard
   - Navigate to **Driver Testing** tab
   - Scroll to **"Step Signal Monitor - Pin 3"**

2. **Click a Preset Button**
   - **10 Slow** - 10 steps with slow speed
   - **50 Medium** - 50 steps at medium speed
   - **100 Fast** - 100 steps at fast speed
   - **200 Full Rev** - Full 360° rotation

3. **Expected Output**
   ```
   [14:32:15.123] 🐢 Step sequence: 10 pulses
   [14:32:15.245] ⚙️ Pulses sent successfully
   ```

4. **What to Observe**
   - Stepper motor shaft rotates
   - Number of clicks matches step count
   - Smooth rotation at medium/fast speeds
   - Precise positioning at slow speed

---

## Part 4: Real-Time Control

### Manual Step Control

1. **Set Step Count** (1-200 steps)
   - Use slider to select number of steps
   - Or use preset buttons

2. **Set Step Speed** (Delay between pulses)
   - Slower delay = Faster stepping
   - Typical range: 5-100ms
   - Lower = faster (up to ~1000 steps/sec)

3. **Send Steps**
   - Click "Send Steps" button
   - Pulse log shows command sent
   - Motor rotates by set amount

### Speed Reference
```
Delay (ms) → Speed (Hz) → Behavior
1          → 1000     → Fastest possible
10         → 100      → Very fast
50         → 20       → Normal speed
100        → 10       → Slow stepping
1000       → 1        → Very slow (1 step/sec)
```

---

## Part 5: Arduino IDE Method

**File**: `Arduino/StepSignalTest_Pin3.ino`

### Automatic Test Sequence

The sketch runs 5 tests:

1. **Slow Stepping** (10 pulses)
   - 1 second between each step
   - Shaft rotates 18° total

2. **Medium Speed** (50 pulses)
   - 100ms between pulses
   - Shaft rotates 90° (quarter turn)

3. **Fast Stepping** (100 pulses)
   - 10ms between pulses
   - Shaft rotates 180° (half turn)

4. **Smooth Rotation** (200 pulses)
   - 5ms between pulses
   - Shaft rotates 360° (full turn)

5. **Pulse Accuracy Test**
   - Verifies precise 2µs pulse width
   - Checks timing accuracy

### How to Use
1. Upload sketch to Arduino
2. Open Serial Monitor (9600 baud)
3. Watch motor rotate through tests
4. Verify each step counts

---

## Part 6: Step Sequence Examples

### Example 1: Quarter Turn (90°)
```
Sequence: Send 50 steps
Expected: Motor rotates 90°
Time: ~5 seconds at 10Hz
```

### Example 2: Full Rotation (360°)
```
Sequence: Send 200 steps
Expected: Motor rotates 360° (1 full turn)
Time: ~20 seconds at 10Hz
```

### Example 3: Precise Positioning
```
Sequence: Send 17 steps (30.6°)
Expected: Motor in new position
Use for: Precise angular control
```

---

## Part 7: Troubleshooting

### Problem: Motor doesn't move

**Cause**: No signal or bad wiring

**Fix**:
1. Check Pin 3 connection to driver
2. Verify driver power supply
3. Check driver configuration (micro-stepping, etc.)
4. Try Arduino IDE sketch first

### Problem: Inconsistent stepping

**Cause**: Timing or pulse width issue

**Fix**:
1. Check pulse timing in serial output
2. Reduce step speed (increase delay)
3. Verify driver is properly powered
4. Check for EMI/noise on signal line

### Problem: Motor moves wrong direction

**Cause**: Direction pin polarity

**Fix**:
1. Check direction signal (separate pin)
2. May need to invert direction signal
3. Verify driver documentation

### Problem: Stepper misses steps

**Cause**: Too fast or insufficient torque

**Fix**:
1. Slow down stepping (increase delay)
2. Reduce motor load
3. Increase motor power supply
4. Check for stalled motor

### Problem: WinUI shows commands but motor doesn't move

**Cause**: Serial communication or Arduino firmware issue

**Fix**:
1. Verify COM port
2. Check Arduino handles STEP_PULSE command
3. Test with Arduino IDE first
4. Check baud rate (9600)

---

## Part 8: Understanding Pulse Timing

### Standard Stepper Driver Pulse Requirements

```
Pulse Width:     2 microseconds minimum
Rise/Fall Time:  <1 microsecond (sharp edges)
Logic High:      5V
Logic Low:       0V (GND)
Frequency Max:   ~10kHz (100,000 steps/sec)
```

### Arduino Pin 3 Capabilities
```
✓ Can generate 2µs pulses
✓ Can handle 10kHz frequency
✓ 5V output (standard logic)
✓ Suitable for all stepper drivers
```

---

## Part 9: Motor Types and Calculations

### NEMA 17 Stepper (200 steps/revolution)
```
Angle per step: 360° ÷ 200 = 1.8°
For 90° rotation: 90 ÷ 1.8 = 50 steps
For 180° rotation: 180 ÷ 1.8 = 100 steps
For 360° rotation: 360 ÷ 1.8 = 200 steps
```

### NEMA 23 Stepper (200 steps/revolution)
```
Same calculation as NEMA 17
```

### With Microstepping (1/2 stepping)
```
Angle per step: 1.8° ÷ 2 = 0.9°
For 90°: 90 ÷ 0.9 = 100 steps (instead of 50)
```

---

## Part 10: Performance Metrics

### Response Time
- Pulse generation: < 10 microseconds
- Command processing: < 100ms
- Motor response: 1-2ms

### Accuracy
- Pulse width: ±10% (1.8-2.2µs)
- Timing precision: Microsecond level
- Step accuracy: 100% (if properly driven)

### Maximum Speed
```
Theoretical: ~10kHz (100,000 steps/sec)
Practical: ~1kHz (1,000 steps/sec)
Recommended: <500 steps/sec
```

---

## Part 11: Testing Procedure

### Full Verification Workflow

```
1. SETUP (2 minutes)
   ☐ Stepper wired to Pin 3 + GND
   ☐ Driver powered correctly
   ☐ Arduino connected via USB
   ☐ WinUI app running
   ☐ COM port selected

2. QUICK TEST (5 minutes)
   ☐ Click "10 Slow" button
   ☐ Count motor steps (should be 10)
   ☐ Listen for audible stepping

3. MEDIUM TEST (5 minutes)
   ☐ Click "50 Medium" button
   ☐ Motor rotates 90° (quarter turn)
   ☐ Check smooth rotation

4. FAST TEST (5 minutes)
   ☐ Click "100 Fast" button
   ☐ Motor rotates 180° (half turn)
   ☐ Check for skipped steps

5. FULL ROTATION (5 minutes)
   ☐ Click "200 Full Rev" button
   ☐ Motor completes full rotation
   ☐ Verify stops at start position

6. VERIFICATION (2 minutes)
   ☐ All tests passed? ✓
   ☐ No skipped steps? ✓
   ☐ Smooth rotation? ✓

TOTAL TIME: ~25 minutes
```

---

## Part 12: Command Reference

### From WinUI App

```
Button             → Command           → Result
"10 Slow"          → STEP_PULSE=10     → 10 steps slow
"50 Medium"        → STEP_PULSE=50     → 50 steps medium
"100 Fast"         → STEP_PULSE=100    → 100 steps fast
"200 Full Rev"     → STEP_PULSE=200    → 200 steps (~360°)
"Send Steps"       → STEP_PULSE=N      → Custom steps
```

### From Serial Console

```
Command Format:     Result:
STEP_PULSE=10       Send 10 step pulses
STEP_PULSE=50       Send 50 step pulses
STEP_PULSE=100      Send 100 step pulses
STEP_PULSE=200      Send 200 step pulses
STEP_PULSE=1        Send 1 step pulse
```

---

## Part 13: Common Step Counts

### Standard Movements

| Steps | Angle | Duration* | Name |
|-------|-------|-----------|------|
| 1 | 1.8° | 10ms | Single step |
| 10 | 18° | 100ms | Test stepping |
| 17 | 30.6° | 170ms | Small angle |
| 50 | 90° | 500ms | Quarter turn |
| 100 | 180° | 1000ms | Half turn |
| 200 | 360° | 2000ms | Full rotation |
| 400 | 720° | 4000ms | Double rotation |

*At 10Hz (100ms between pulses)

---

## Part 14: Visual Indicators

### Step Status Display

```
⏸️ IDLE      = No steps being sent
⚙️ STEPPING  = Steps in progress
✅ COMPLETE  = Steps finished
⚡ FAST      = High frequency stepping
```

### Pulse Log Entries

```
[14:32:15.123] 🐢 Step sequence: 10 pulses     ← Slow
[14:32:15.245] 🚗 Step sequence: 50 pulses     ← Medium
[14:32:15.367] 🚀 Step sequence: 100 pulses    ← Fast
[14:32:15.489] ⚡ Step sequence: 200 pulses    ← Full rotation
```

---

## Part 15: Safety Notes

⚠️ **Important Safety Information**:

- Don't block stepper shaft while running
- Motor will be hot after continuous operation
- Stepper can hold position indefinitely
- Don't exceed 10kHz pulse rate without testing
- Use proper shielded cables for stepper signals
- Stepper driver may have protective features
- Test with light mechanical load first

---

## Part 16: Advanced Tips

### Precise Positioning
```
Use small step counts (1-10 steps)
Verify position with each command
Build up motion gradually
Good for: Fine angle adjustments
```

### Fast Rotation
```
Start with 50ms delay (20 Hz)
Increase speed gradually
Stop at 5-10ms delay (100-200 Hz)
Good for: Quick repositioning
```

### Smooth Continuous Motion
```
Use 10ms delay (100 Hz)
Send 200+ steps for rotation
Consistent timing is key
Good for: Smooth sweeping motions
```

---

## Success Example

### Correct Test Results
```
Test Sequence:
[14:32:15] ▶ Testing stepper motor on Pin 3
[14:32:16] 🐢 Step sequence: 10 pulses
[14:32:17] Motor rotated slightly (18°)
[14:32:18] 🚗 Step sequence: 50 pulses
[14:32:19] Motor rotated 90° (quarter turn)
[14:32:20] 🚀 Step sequence: 100 pulses
[14:32:21] Motor rotated 180° (half turn)
[14:32:22] ⚡ Step sequence: 200 pulses
[14:32:23] Motor completed full 360° rotation

Result: ✅ Stepper motor working perfectly
```

---

## Part 17: Quick Reference

| Task | Location | Action |
|------|----------|--------|
| Quick Test | Driver Testing → Step Monitor | Click "10 Slow" |
| Real-Time Control | Step Monitor → Sliders | Adjust and "Send Steps" |
| Arduino Method | Arduino/StepSignalTest_Pin3.ino | Upload & monitor |
| Send Command | Serial Console | STEP_PULSE=value |
| Custom Sequence | Step Monitor | Set steps + speed |

---

## Need Help?

### Common Questions

**Q: How many steps for a 90° turn?**  
A: 90 ÷ 1.8 = 50 steps (for standard 200-step motors)

**Q: What's a safe maximum speed?**  
A: Start at 50ms delay (20Hz), increase gradually

**Q: Why is my motor skipping steps?**  
A: Too fast, insufficient power, or driver issue

**Q: Can I use Pin 3?**  
A: Yes, Pin 3 is a standard digital output

**Q: What if motor moves in wrong direction?**  
A: Swap direction signal polarity

---

```
╔════════════════════════════════════════════════════════════════╗
║                                                                ║
║         ⚙️ STEP SIGNAL TESTING SYSTEM - READY TO USE           ║
║                                                                ║
║  Professional stepper motor control and testing               ║
║  Precise positioning capabilities                            ║
║  Real-time command execution                                 ║
║  Comprehensive feedback and logging                          ║
║                                                                ║
║              START WITH PRESET BUTTONS FOR BEST RESULTS       ║
║                                                                ║
╚════════════════════════════════════════════════════════════════╝
```

---

**Last Updated**: December 2024  
**Firmware Version**: Step Signal Test v1.0  
**Compatible Boards**: Arduino Uno (Pin 3)  
**Test Interface**: WinUI Smart Footwear Dashboard
