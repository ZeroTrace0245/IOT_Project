# Limit Switch Testing Guide - WinUI Application

## Overview
This guide explains how to test your limit switch connected to **Pin 4** using the **WinUI Dashboard** application.

---

## Part 1: Hardware Setup

### Wiring Diagram
```
Limit Switch
├─ Pin 1 (Signal) ──→ Arduino Pin 4
└─ Pin 2 (Ground) ──→ Arduino GND
```

### Verification Checklist
- [ ] Limit switch physically wired to Pin 4
- [ ] Ground wire connected to Arduino GND
- [ ] Arduino connected via USB to computer
- [ ] WinUI application running
- [ ] Serial port detected in application

---

## Part 2: Quick Test (Single Press)

### Option 1: Quick Test Option (Test 4)

1. **Launch the WinUI App**
   - Open the Smart Footwear Dashboard
   - Navigate to **Driver Testing** tab

2. **Select Test 4: Limit Switch**
   - From the dropdown menu, select `4. Limit Switch`
   - Click **▶ Run Test**

3. **Expected Output**
   ```
   [TEST 4] Limit Switch Detection
   Reading limit switch state...

   Limit Switch State: HIGH (NOT TRIGGERED - expected at rest)
   ✓ Check: Press the limit switch and it should read LOW

   Monitoring for 2 seconds...
   ✓ State changed to: LOW
   ✓ Limit switch is working correctly!
   ```

4. **What to Look For**
   - Initial state shows **HIGH** (switch open)
   - State changes to **LOW** when you press the switch
   - Confirmation message appears

---

## Part 3: Real-Time Monitoring (Continuous)

### Option 2: Dedicated Monitor (Test 7 - Recommended)

This is the best way to test and verify your limit switch continuously.

#### Step 1: Start the Monitor

1. Go to **Driver Testing** tab
2. Select `7. Limit Switch Monitor` from dropdown
3. Click **▶ Start Monitor**

#### Step 2: What You'll See

**Displayed Information:**
- **Current State Display** (large indicator)
  - 🔴 **PRESSED (LOW)** - Shows in red when switch is pressed
  - 🟢 **OPEN (HIGH)** - Shows in green when switch is open

- **Statistics Panel**
  - **Presses Detected**: Increments each time switch pressed
  - **Last Event**: Timestamp of most recent press/release

- **Event Log**: Detailed timeline of all state changes
  ```
  [HH:mm:ss.fff] ▶ Monitoring started...
  [HH:mm:ss.fff] 🔴 PRESSED - Press #1
  [HH:mm:ss.fff] 🟢 RELEASED
  [HH:mm:ss.fff] 🔴 PRESSED - Press #2
  [HH:mm:ss.fff] 🟢 RELEASED
  ```

#### Step 3: Test Procedure

1. **Initial State Check**
   - Monitor should show: **🟢 OPEN (HIGH)**
   - Presses Detected: **0**

2. **Press the Limit Switch**
   - Display changes to: **🔴 PRESSED (LOW)**
   - Presses Detected: **1**
   - Event log shows: `🔴 PRESSED - Press #1`

3. **Release the Switch**
   - Display changes to: **🟢 OPEN (HIGH)**
   - Event log shows: `🟢 RELEASED`

4. **Repeat Multiple Times**
   - Press and release 5-10 times
   - Verify counter increases each press
   - Check that state changes are **instant**

5. **Stop Monitoring**
   - Click **⏹ Stop Monitor**
   - Display shows: **Stopped**
   - Monitor button re-enables

#### Step 4: Verify Results

| Check | Expected | Status |
|-------|----------|--------|
| Initial state | 🟢 OPEN (HIGH) | ✓ |
| Press detected | 🔴 PRESSED (LOW) | ✓ |
| Release detected | 🟢 OPEN (HIGH) | ✓ |
| Counter increments | Increases per press | ✓ |
| State change timing | Instant (< 50ms) | ✓ |
| No state flicker | Stable display | ✓ |

---

## Part 4: Troubleshooting

### Problem: Monitor shows "--- (Not Connected)"

**Cause**: Arduino not connected or serial communication issue

**Fix**:
1. Check Arduino USB connection
2. Select correct COM port from dropdown
3. Click **Connect** button
4. Retry Test 7

### Problem: State stays HIGH, doesn't change to LOW

**Cause**: Limit switch not physically connected or bad wiring

**Fix**:
1. Verify wires at both ends
2. Check continuity with multimeter
3. Press switch firmly - it should be a mechanical click
4. Try wiggling wires while monitoring

### Problem: State flickers or bounces

**Cause**: Switch contact bouncing (normal for older switches)

**Fix**:
- This is typically handled by Arduino firmware debouncing
- If severe, contact is likely dirty - clean with pencil eraser

### Problem: Presses detected is 0 after multiple presses

**Cause**: State change detection issue

**Fix**:
1. Check if real-time states are updating
2. Make sure you see both HIGH and LOW on the display
3. Verify event log shows entries for each press

---

## Part 5: Using with Serial Console

For advanced users who want custom commands:

### Command: READ_SWITCH

```
Send: READ_SWITCH
Response: SWITCH_STATE=HIGH
```

Or when pressed:
```
Send: READ_SWITCH  
Response: SWITCH_STATE=LOW
```

### Command: MONITOR_SWITCH=START

Starts continuous monitoring (used by Test 7 automatically)

```
Send: MONITOR_SWITCH=START
Response: [periodic SWITCH_STATE updates every 50ms]
```

### Command: MONITOR_SWITCH=STOP

Stops continuous monitoring

```
Send: MONITOR_SWITCH=STOP
Response: STATUS=Monitor stopped
```

---

## Part 6: Understanding the State Values

### HIGH (1) = Switch OPEN
- Switch is **NOT** pressed
- Normal resting state
- Green indicator (🟢)
- Voltage: ~5V at Pin 4

### LOW (0) = Switch PRESSED
- Switch **IS** pressed / activated
- Mechanical contact made
- Red indicator (🔴)
- Voltage: ~0V at Pin 4

---

## Part 7: Technical Details

### Pin Configuration
```
Pin 4: INPUT_PULLUP
- Internal pullup resistor enabled
- Normally reads HIGH
- Pulled to LOW when switch closes
```

### Update Rate
- Monitor polling: Every 50ms
- Event log capture: Real-time
- Display refresh: UI thread

### Statistics Tracked
- Press count (increments per LOW detection)
- Timestamp of each state change
- Event history (up to 100 most recent events)

---

## Part 8: Complete Test Workflow

### Full Verification Procedure

```
1. SETUP (2 minutes)
   □ Wire limit switch to Pin 4 + GND
   □ Power Arduino via USB
   □ Launch WinUI app
   □ Select COM port and connect

2. QUICK TEST (1 minute) - Test 4
   □ Select "4. Limit Switch"
   □ Click "Run Test"
   □ Press switch once
   □ Verify state changes

3. DETAILED TEST (3 minutes) - Test 7
   □ Select "7. Limit Switch Monitor"
   □ Click "Start Monitor"
   □ Press switch 10 times
   □ Check press counter
   □ Check event log
   □ Click "Stop Monitor"

4. VALIDATION (1 minute)
   □ All tests passed? ✓
   □ No state flicker? ✓
   □ Counter accurate? ✓
   □ Timing instant? ✓

TOTAL TIME: ~7 minutes
```

---

## Part 9: Advanced Features

### Export Event Log
While monitoring, you can copy the event log for documentation:
1. Right-click in Event Log area
2. Select all text
3. Copy and save to file

### Real-Time Analysis
The WinUI app shows:
- Precise timestamps down to milliseconds
- State change frequency
- Press detection reliability

### Arduino IDE Comparison
Compare results with Arduino IDE Serial Monitor to verify both methods work identically.

---

## Part 10: Success Criteria ✓

Your limit switch is **working correctly** if:

✅ Initial state displays as **🟢 OPEN (HIGH)**

✅ Pressing switch immediately displays **🔴 PRESSED (LOW)**

✅ Releasing switch immediately displays **🟢 OPEN (HIGH)**

✅ Press counter increments for each press

✅ Event log shows all state changes with timestamps

✅ No state flickering or bouncing visible

✅ State changes in < 100ms

✅ Multiple press/release cycles work consistently

---

## Quick Reference

| Task | Location | Action |
|------|----------|--------|
| Quick Test | Driver Testing → Test 4 | Click "Run Test" |
| Continuous Monitor | Driver Testing → Test 7 | Click "Start Monitor" |
| Custom Commands | Serial Console → Custom Command | Type command + click "SEND" |
| View Detailed Logs | Serial Console → Output Log | Scroll through entries |
| Check Connection | Sidebar → Status | Look for green indicator |

---

## Need Help?

### Check This First
1. Is Arduino USB connected? (look for COM port in dropdown)
2. Is limit switch physically pressed/released correctly?
3. Are wires connected to Pin 4 and GND?
4. Is serial connection showing as "Connected" (green)?

### Common Issues
- **No state change**: Check wiring and switch mechanism
- **Connection issues**: Verify COM port and baud rate (9600)
- **Flicker**: May be normal switch bounce - check firmware debouncing
- **No incrementing counter**: Ensure state changes from HIGH to LOW

---

**Last Updated**: December 2024  
**Firmware Version**: Arduino Diagnostics v1.0  
**Compatible Boards**: Arduino Uno with TMC2209 Driver  
**Test Interface**: WinUI Smart Footwear Dashboard
