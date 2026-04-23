# 🎯 TMC2209 Driver Identification & Setup - Complete Guide

## 📚 Documentation Structure

You now have 4 complete guides + 3 test sketches:

### 📖 Documentation Files
1. **QUICK_VERIFICATION_GUIDE.md** ← START HERE
2. **WIRING_VERIFICATION_CHECKLIST.md** ← Visual reference
3. **DRIVER_TESTING_GUIDE.md** ← Complete troubleshooting
4. **THIS FILE** - Master overview

### 🧪 Arduino Sketches
1. **Arduino/DriverDiagnostics.ino** - Comprehensive automated tests
2. **Arduino/QuickDriverTest.ino** - Interactive command-based testing
3. **Arduino/FootScanner.ino** - Production firmware

---

## 🚀 Quick Start (5 Minutes)

### Step 1: Visual Check (2 min)
```
☐ 12V power connected to VM
☐ GND connected to GND
☐ 100µF capacitor installed (+ to VM, - to GND)
☐ Motor 4 wires connected to 1A, 1B, 2A, 2B
☐ Arduino pins 2, 3, 4, 5, A0 wired to driver
☐ Green LED on driver board visible
```

### Step 2: Upload Test Sketch (2 min)
```
1. Open Arduino IDE
2. File → Arduino/DriverDiagnostics.ino
3. Select Board: Arduino Uno
4. Select Port: COM# (your device)
5. Click Upload (Ctrl+U)
6. Wait for "Done uploading"
```

### Step 3: Run Tests (1 min)
```
1. Tools → Serial Monitor (Ctrl+Shift+M)
2. Set Speed: 9600 baud
3. Watch automatic tests run
4. Read results
```

**Result**: 
- ✅ All tests pass → Driver is working!
- ❌ Some tests fail → Check wiring (see guides)

---

## 📋 What Each Test Does

### Test 1: Motor Enable/Disable ⚡
```
✓ Sets EN pin HIGH (motor disabled)
✓ Sets EN pin LOW (motor enabled)
✓ Check: Green LED lights when enabled
✓ Check: Motor becomes hard to turn when enabled
```

### Test 2: Direction Control ↔️
```
✓ Toggles DIR pin HIGH (forward) and LOW (reverse)
✓ Check: Motor can reverse direction smoothly
```

### Test 3: Step Signal 🔄
```
✓ Sends 50 step pulses forward
✓ Sends 50 step pulses backward
✓ Check: Motor rotates ~0.25 revolution forward, then back
```

### Test 4: Limit Switch 🔔
```
✓ Reads limit switch state (should be HIGH)
✓ Waits for you to press the switch (should go LOW)
✓ Check: Detects state changes when switch pressed
```

### Test 5: Pressure Sensor 📊
```
✓ Reads analog pressure sensor 5 times
✓ Check: Values between 0-1023
✓ Check: Values change with pressure applied
```

### Test 6: Full Rotation 🎡
```
✓ Rotates motor 1 full revolution (200 steps) forward
✓ Rotates motor 1 full revolution (200 steps) backward
✓ Check: Motor returns to starting position
```

---

## 🎯 Success Indicators

| Check | Status | Action |
|-------|--------|--------|
| Green LED lights | ✅ ON | Good: Power and enable working |
| Motor locks when enabled | ✅ STIFF | Good: Coils are energized |
| Motor rotates smoothly | ✅ SMOOTH | Good: Step signal working |
| Limit switch detects press | ✅ DETECTED | Good: Input sensor working |
| Pressure sensor reads values | ✅ 0-1023 | Good: ADC working |
| Full rotation works | ✅ COMPLETE | Good: All systems synchronized |

**All 6 ✅ = Your driver is working perfectly!**

---

## ❌ Common Problems & Solutions

### Problem 1: Green LED Doesn't Light
```
Cause: No power to driver
Fixes:
  ✓ Check 12V supply is plugged in
  ✓ Verify GND connection
  ✓ Check 100µF capacitor is installed
  ✓ Measure voltage: Should see ~12V from VM to GND
```

### Problem 2: Motor Doesn't Move
```
Cause: Step signal not working or motor disabled
Fixes:
  ✓ Check Pin 3 (STEP) is wired correctly
  ✓ Verify EN pin goes LOW (motor enabled)
  ✓ Try QuickDriverTest.ino for manual testing
  ✓ Test: Send command "3" to step 20 times
```

### Problem 3: Motor Vibrates Only
```
Cause: Motor coil wiring is wrong
Fixes:
  ✓ Swap RED ↔ BLUE (reverse one coil)
  ✓ OR swap BLACK ↔ GREEN
  ✓ Retry: Test command "3" or "6"
  ✓ Motor should rotate smoothly now
```

### Problem 4: Limit Switch Not Detected
```
Cause: Switch wiring or pin issue
Fixes:
  ✓ Check Pin 2 connection
  ✓ Verify switch has 3 wires: Red (5V), Black (GND), Yellow (Pin 2)
  ✓ Try: QuickDriverTest.ino command "5"
  ✓ Press switch and watch Serial Monitor
  ✓ Should see state change from HIGH to LOW
```

### Problem 5: Pressure Sensor Shows 0 or 1023
```
Cause: Sensor not connected or broken
Fixes:
  ✓ Check A0 connection
  ✓ Verify sensor has power (red wire to 5V)
  ✓ Check ground connection (black wire to GND)
  ✓ Try: QuickDriverTest.ino command "4"
  ✓ Should show values changing 0-1023
```

### Problem 6: No Serial Communication
```
Cause: COM port or baud rate wrong
Fixes:
  ✓ Arduino IDE → Tools → Port → Select COM#
  ✓ Serial Monitor → Set to 9600 baud
  ✓ Verify USB cable is firmly connected
  ✓ Try: Tools → Get Board Info
  ✓ Restart Arduino IDE if needed
```

---

## 🧪 Testing Strategy

### Option A: Automated (Easiest)
1. Upload: `DriverDiagnostics.ino`
2. Open Serial Monitor
3. Watch tests run automatically
4. Read results

**Best for: First verification, comprehensive check**

### Option B: Interactive (Manual Control)
1. Upload: `QuickDriverTest.ino`
2. Open Serial Monitor
3. Type commands: 1, 2, 3, 4, 5, 6
4. Test one component at a time

**Best for: Debugging specific problems, manual testing**

### Option C: Production (Full System)
1. Upload: `FootScanner.ino`
2. Open WinUI Dashboard
3. Select COM port (9600 baud)
4. Click "Measure"

**Best for: After verification, real measurements**

---

## 📊 Expected Serial Output

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
ENABLE_PIN set to HIGH (motor DISABLED)
ENABLE_PIN set to LOW (motor ENABLED)
✓ Check: Motor should be harder to turn now
✓ Indicator: Green LED on driver should light up

[TEST 2] Direction Control
DIR_PIN set to HIGH (FORWARD direction)
DIR_PIN set to LOW (REVERSE direction)
✓ Check: Motor should be able to switch directions

[TEST 3] Step Signal / Motor Movement
Sending 50 step pulses (forward)...
✓ 50 steps sent
✓ Check: Motor shaft should have rotated slightly
Sending 50 step pulses (reverse)...
✓ 50 reverse steps sent
✓ Check: Motor should have returned to original position

[TEST 4] Limit Switch Detection
Limit Switch State: HIGH (NOT TRIGGERED)
✓ Check: Press the limit switch and it should read LOW
Monitoring for 2 seconds...
✓ State changed to: LOW    ← (appears when you press switch)

[TEST 5] Pressure Sensor (A0)
Reading pressure sensor 5 times...
Reading 1: RAW=42 (0-1023) | Voltage=0.21V
Reading 2: RAW=45 (0-1023) | Voltage=0.22V
Reading 3: RAW=48 (0-1023) | Voltage=0.23V
Reading 4: RAW=44 (0-1023) | Voltage=0.22V
Reading 5: RAW=46 (0-1023) | Voltage=0.23V

[TEST 6] Full Motor Movement Sequence
Executing 200 steps (1 full revolution)...
→ FORWARD: 200 steps
✓ Completed
← REVERSE: 200 steps
✓ Completed
Motor test complete!

========================================
(Cycle repeats every 30 seconds)
```

---

## ✅ Final Verification Checklist

Before declaring success:

```
□ Green LED lights up when motor is enabled
□ Motor shaft locks/becomes stiff when enabled
□ Motor shaft turns freely when disabled
□ Motor rotates smoothly when stepping forward
□ Motor rotates smoothly when stepping backward
□ Motor returns to original position after forward+reverse
□ Limit switch reads HIGH at rest
□ Limit switch reads LOW when pressed
□ Pressure sensor reads 0-1023 depending on pressure
□ Full 200-step rotation completes without errors
□ All serial output shows no error messages
□ Tests repeat successfully for multiple cycles
```

**If ALL ✓ checks pass → Your TMC2209 driver is properly identified and fully functional!**

---

## 🚀 Next Steps

### After Verification Passes:

1. **Upload Production Firmware**
   ```
   Arduino → Upload Arduino/FootScanner.ino
   ```

2. **Test with WinUI Dashboard**
   ```
   Visual Studio → Start Debugging (F5)
   Select COM port in dashboard
   Click "Measure" button
   ```

3. **Calibrate System**
   ```
   Measure known foot size
   Verify results accuracy
   Adjust if needed
   ```

4. **Deploy**
   ```
   Run complete measurement cycle
   Verify QR codes display
   Test cashier section
   ```

---

## 📞 Support Resources

### If Tests Still Fail:

1. **Check WIRING_VERIFICATION_CHECKLIST.md** for pin locations
2. **Check DRIVER_TESTING_GUIDE.md** for troubleshooting
3. **Use QuickDriverTest.ino** for manual step-by-step testing
4. **Take photos** of your wiring and compare with diagrams
5. **Measure voltages** with multimeter if available

### Key Things to Verify:

- [ ] 100µF capacitor installed (CRITICAL!)
- [ ] Motor wires in correct order (1A, 1B, 2A, 2B)
- [ ] Pin connections exact (3, 4, 5, 2, A0)
- [ ] 9600 baud rate in Serial Monitor
- [ ] Green LED visible on driver board
- [ ] 12V power supply connected

---

## 🎉 Success!

**You now have the complete system to verify your TMC2209 driver!**

Start with **QUICK_VERIFICATION_GUIDE.md** and follow the steps. You'll know within 5 minutes if your driver is working correctly.

**Everything is synchronized and ready to go!** 🚀

---

### File Reference
- 📍 **Arduino/DriverDiagnostics.ino** - Full automated tests
- 📍 **Arduino/QuickDriverTest.ino** - Interactive testing
- 📍 **Arduino/FootScanner.ino** - Production firmware
- 📄 **QUICK_VERIFICATION_GUIDE.md** - START HERE
- 📄 **WIRING_VERIFICATION_CHECKLIST.md** - Visual reference
- 📄 **DRIVER_TESTING_GUIDE.md** - Complete guide

### WinUI Application
- 📁 **MainWindow.xaml.cs** - Dashboard communication (9600 baud already configured ✓)
- 📁 **MainWindow.xaml** - UI with measurement, cashier, custom design pages

---

**Happy testing! Your TMC2209 driver verification is now complete.** 🎯
