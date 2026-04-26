# How to Identify & Verify TMC2209 Driver Is Working

## 🎯 Quick Summary

You have **3 test sketches** to verify your TMC2209 driver:

### 1. **DriverDiagnostics.ino** (COMPREHENSIVE - Recommended)
- **Use this for complete system testing**
- Tests all 6 components automatically
- Provides detailed output and troubleshooting tips
- **Best for first-time verification**

### 2. **QuickDriverTest.ino** (INTERACTIVE - For Live Testing)
- **Use this for manual control**
- Type commands in Serial Monitor: 1, 2, 3, 4, 5, 6
- Test one component at a time
- **Best for troubleshooting specific problems**

### 3. **FootScanner.ino** (PRODUCTION - Main Firmware)
- **Use this after verification passes**
- Full foot measurement system
- Communicates with WinUI dashboard

---

## 📋 Which Sketch to Upload First?

### ✅ START HERE:

```
1. Upload: Arduino/DriverDiagnostics.ino
   └─ Opens Serial Monitor (9600 baud)
   └─ Watches 6 automated tests
   └─ Takes ~30 seconds

2. If all tests PASS ✓
   └─ Upload: Arduino/FootScanner.ino
   └─ Connect to WinUI Dashboard
   └─ Run foot measurements

3. If any test FAILS ✗
   └─ Check wiring (see guide below)
   └─ Try: Arduino/QuickDriverTest.ino
   └─ Debug specific components
```

---

## 🔍 Visual Signs Your Driver Is Working

### ✅ Good Signs
- **Green LED lights up** on TMC2209 when motor is enabled
- **Motor shaft locks** (hard to turn) when enabled
- **Motor shaft is free** to turn when disabled
- **Motor rotates smoothly** when steps are sent
- **Motor reverses direction** when DIR pin toggles

### ❌ Bad Signs
- **No LED on driver board** → Power problem
- **Motor doesn't lock when enabled** → Enable pin not working
- **Motor vibrates but doesn't spin** → Wrong motor coil wiring
- **No motor movement** → Step signal not working
- **Motor moves in wrong direction** → DIR pin not working

---

## 🧪 Step-by-Step Verification

### Step 1: Visual Check (2 minutes)
```
☐ Power is connected (12V to VM, GND to GND)
☐ 100µF capacitor is installed (+ to VM, - to GND)
☐ Arduino pins connected:
	☐ Pin 3 → STEP
	☐ Pin 4 → DIR
	☐ Pin 5 → EN
	☐ Pin 2 → LIMIT_SWITCH
	☐ A0 → PRESSURE_SENSOR
☐ Motor 4 wires connected:
	☐ Black → 1A
	☐ Green → 1B
	☐ Red → 2A
	☐ Blue → 2B
```

### Step 2: Upload Diagnostic Sketch (5 minutes)
```
1. In Arduino IDE:
   - File → Open → Arduino/DriverDiagnostics.ino
   - Select: Tools → Board: Arduino Uno
   - Select: Tools → Port: COM# (your Arduino)
   - Click: Upload (Ctrl+U)

2. Wait for upload to complete
   - Status: "Done uploading"
```

### Step 3: Open Serial Monitor (2 minutes)
```
1. In Arduino IDE:
   - Tools → Serial Monitor (Ctrl+Shift+M)
   - Set speed: 9600 baud (bottom right)
   - Auto-scroll: checked
```

### Step 4: Watch Tests Run (30 seconds)
```
The sketch will automatically:
✓ Test 1: Motor Enable/Disable
✓ Test 2: Direction Control  
✓ Test 3: Step Signal
✓ Test 4: Limit Switch
✓ Test 5: Pressure Sensor
✓ Test 6: Full Motor Movement

Each test takes ~5 seconds
```

### Step 5: Read Results
```
PASS ✅ All 6 tests complete without errors
   → Driver is working! Upload FootScanner.ino

FAIL ❌ One or more tests show errors
   → Check wiring and try QuickDriverTest.ino
   → See troubleshooting guide below
```

---

## 🔧 Troubleshooting: What to Do If Tests Fail

### Problem 1: "No Serial Output"
**Cause**: COM port or baud rate wrong

**Fix**:
1. Check: Tools → Port → Select correct COM#
2. Close and reopen Serial Monitor
3. Verify 9600 baud is selected
4. Try: Arduino → Tools → Get Board Info

---

### Problem 2: "Green LED Doesn't Light"
**Cause**: Power not reaching driver

**Fix**:
1. Check: 12V power supply is plugged in
2. Verify: GND is connected from power to driver
3. Check: 100µF capacitor is installed correctly
4. Measure voltage: Should see ~12V from VM to GND

---

### Problem 3: "Motor Doesn't Move / Vibrates Only"
**Cause**: Step signal not reaching motor OR coil wiring wrong

**Fix**:
1. **If no movement at all**:
   - Check Pin 3 (STEP) connection
   - Verify motor is enabled (EN=LOW)
   - Try: QuickDriverTest → Command 3

2. **If motor vibrates but doesn't spin**:
   - Swap motor wires: RED ↔ BLUE
   - OR swap: BLACK ↔ GREEN
   - Try command 3 again

---

### Problem 4: "Limit Switch Not Detected"
**Cause**: Switch wiring or pin issue

**Fix**:
1. Check Pin 2 connection to limit switch
2. Verify switch works manually:
   - Should click when pressed
   - Should feel resistance
3. Try: QuickDriverTest → Command 5
4. Watch Serial Monitor while pressing switch
5. State should change from HIGH to LOW

---

### Problem 5: "Pressure Sensor Stuck at 0 or 1023"
**Cause**: Sensor not connected or out of range

**Fix**:
1. Check A0 connection to pressure sensor
2. Verify sensor has power (red wire) and ground (black wire)
3. Try: QuickDriverTest → Command 4
4. Should show values between 0-1023
5. Values changing means sensor is working

---

## 📊 Expected Values

| Component | Expected Value | What It Means |
|-----------|---|---|
| Green LED | **ON** when EN=LOW | Driver is powered and enabled |
| Motor Lock | **Stiff** when EN=LOW | Motor coils are energized |
| Step Signal | **Pulse** 50-500µs | Driver receives stepping commands |
| Limit Switch | **HIGH** at rest, **LOW** when pressed | Switch works correctly |
| Pressure Sensor | **0-1023** when unpressed | ADC range is correct |
| Motor Rotation | **Smooth** forward/backward | Motor and driver synchronized |

---

## ✅ Success Checklist

Once all tests pass, you should see:

- [x] Green LED lights when motor enabled
- [x] Motor shaft locks when enabled
- [x] 50 steps = ~0.25 revolution
- [x] 200 steps = 1 full revolution
- [x] Motor returns to original position after forward+reverse
- [x] Limit switch detects press (state changes)
- [x] Pressure sensor reads 0-1023
- [x] All serial output is clear with no errors

**If ALL ✓ checks pass → Your driver is properly identified and working!**

---

## 🚀 Next Steps After Verification

1. **Upload Main Firmware**:
   - Arduino/FootScanner.ino

2. **Test with WinUI Dashboard**:
   - Open IOT APP in Visual Studio
   - Select COM port
   - Click "Measure"
   - Watch FOOT= values in Serial Monitor

3. **Calibration** (optional):
   - Measure with known size
   - Adjust sensitivity if needed
   - Verify QR codes display correctly

4. **Deploy**:
   - Test complete measurement cycle
   - Verify data saves correctly
   - Ready for production use

---

## 📞 Still Having Issues?

1. Take a photo of your wiring
2. Check Arduino IDE → Serial Monitor output
3. Compare with expected values table above
4. Verify each connection one by one
5. Try QuickDriverTest.ino for live debugging

**You're now equipped to verify your TMC2209 driver works perfectly!** 🎉
