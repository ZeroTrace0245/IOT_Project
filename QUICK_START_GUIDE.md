# Sensor Detection System - Quick Start Guide

## 🚀 Quick Start in 5 Steps

### **Step 1: Connect Arduino**
1. Go to **Sensor Detection** tab
2. Select your **COM port** from dropdown
3. Click **"Connect"** button
4. Status should show **"Connected"** (green badge)

### **Step 2: Click "Start Handshake"**
Watch the magic happen:
- 🟡 System probes Limit Switch (Pin 4)
- 🟡 System probes Obstacle Sensor (Pin 3)
- 🟡 System probes Ultrasonic Sensor (Pins 1/2)
- 🟡 System probes Pressure Sensor (A0)

### **Step 3: Check Serial Console**
Real-time output shows:
```
[15:30:42] 🔄 Starting sensor detection handshake...
[15:30:42] ▶ Sensor LimitSwitch: Sending 'LIMIT_CHECK'
[15:30:42] << Response: LIMIT_STATE=1
[15:30:42] ✓ LimitSwitch DETECTED - Value: 1
[15:30:42] ▶ Sensor Obstacle: Sending 'OBSTACLE_CHECK'
...
[15:30:45] ✓ Handshake Complete (1250ms)
[15:30:45] 📊 Connected: 4/4 sensors
```

### **Step 4: View Sensor Data**
Each sensor card shows:
- **Status:** ✓ DETECTED or ✗ Not Detected
- **Values:** Actual readings from sensors
- **Ranges:** Safe operating ranges

### **Step 5: Validate & Continue**
- Click **"Validate Sensors"** to double-check
- Click **"Reset Detection"** to start over
- Continue with your foot scan operation

---

## 📊 What Each Sensor Shows

### **Limit Switch** (Pin 4)
```
Status: ✓ DETECTED
State: 1
Meaning: Released (1=Released, 0=Pressed)
```

### **Obstacle Sensor** (Pin 3) - HW201
```
Status: ✓ DETECTED
Detection: ✓ CLEAR
Meaning: No obstacle detected
Range: 2-40 cm
```

### **Ultrasonic** (Pins 1/2)
```
Status: ✓ DETECTED
Distance: 45 cm
Range: 2-400 cm
```

### **Pressure** (Pin A0)
```
Status: ✓ DETECTED
Raw Value: 523
Voltage: 2.55V
Range: 0-1023 (0-5V)
```

---

## 🎛️ Control Buttons

| Button | What It Does |
|--------|-------------|
| **Start Handshake** | Begins sensor detection sequence |
| **Validate Sensors** | Re-checks all connected sensors |
| **Reset Detection** | Clears console and resets all data |
| **Help** | Shows usage guide in console |
| **Clear** | Clears serial console output |

---

## 📈 Status Summary

Shows 3 key metrics:

```
┌─────────────────────────────────────────┐
│ Connected Sensors: 4/4                  │
│ System Status: Complete                 │
│ Handshake Time: 1250ms                  │
└─────────────────────────────────────────┘
```

---

## ❌ Troubleshooting

### **"Arduino not connected!" error**
✅ Solution: Connect Arduino and select correct COM port first

### **Sensor shows "No response (timeout)"**
✅ Check:
- Sensor is physically connected
- Arduino sketch responds to commands
- Wiring is correct
- Arduino is powered on

### **Unexpected response error**
✅ Check Arduino sketch sends exact response format:
- `LIMIT_STATE=X`
- `OBSTACLE_STATE=X`
- `ULTRASONIC_DISTANCE=XX`
- `PRESSURE_RAW=XXX`

### **Console shows commands but no responses**
✅ Check:
- Serial monitor/plotter not open (closes port)
- Arduino sketch not handling commands
- Baud rate is 9600

### **Only some sensors detected**
✅ Check connections for undetected sensors

### **Takes longer than 10 seconds**
✅ Normal! Timeout is 2 seconds per sensor × 4 sensors = 8 seconds max + processing

---

## 🎯 Expected Behavior

### **Normal Handshake (All 4 Sensors)**
```
⏱️ Time: ~9 seconds
✅ Result: "Connected: 4/4 sensors"
🟢 Status: Complete
```

### **3 Sensors Connected**
```
⏱️ Time: ~7 seconds
⚠️ Result: "Connected: 3/4 sensors"
🟡 Status: Partial detection - check missing sensor
```

### **No Sensors Connected**
```
⏱️ Time: ~8 seconds
❌ Result: "Connected: 0/4 sensors"
🔴 Status: No sensors detected - check Arduino
```

---

## 🔧 Arduino Setup Required

Your Arduino sketch must respond to these exact commands:

| Command | Response | Example |
|---------|----------|---------|
| `LIMIT_CHECK` | `LIMIT_STATE=0` or `1` | Pressed or released |
| `OBSTACLE_CHECK` | `OBSTACLE_STATE=0` or `1` | Detected or clear |
| `ULTRASONIC_READ` | `ULTRASONIC_DISTANCE=XX` | Distance in cm |
| `PRESSURE_READ` | `PRESSURE_RAW=XXX` | ADC value 0-1023 |

**See ARDUINO_COMMANDS_REFERENCE.md for code examples**

---

## 💡 Pro Tips

1. **Run detection before foot scan** - Ensures all sensors working
2. **Check console messages** - They tell you exactly what's happening
3. **Use "Validate Sensors"** - Quick re-check without full detection
4. **Note the handshake time** - Baseline for diagnosing issues
5. **Clear console before new run** - Keeps things organized

---

## 📱 UI Layout

```
┌─────────────────────────────────────────┐
│ Sensor Detection & Validation System    │
├─────────────────────────────────────────┤
│ [Start Handshake] [Validate] [Reset] [Help]
├─────────────────────────────────────────┤
│ ┌─────────────┬──────────────┬──────────┐
│ │Connected 0/4│Status: Idle  │Time: 0ms │
│ └─────────────┴──────────────┴──────────┘
├─────────────────────────────────────────┤
│ Detected Sensors [List View]            │
├─────────────────────────────────────────┤
│ 📡 Serial Console [Clear]               │
│ ┌─────────────────────────────────────┐ │
│ │ [HH:mm:ss] 🔄 Starting handshake   │ │
│ │ [HH:mm:ss] ▶ Checking Sensor...    │ │
│ │ [HH:mm:ss] ✓ Sensor DETECTED       │ │
│ │ ...                                 │ │
│ └─────────────────────────────────────┘ │
├─────────────────────────────────────────┤
│ Sensor Information & Status             │
│ ┌────────┬────────┬────────┬────────┐  │
│ │Pressure│ Limit  │Obstacle│Ultrasonic
│ │DETECTED│DETECTED│DETECTED│DETECTED│  │
│ │Raw: 523│State:1 │Clear   │45 cm   │  │
│ │2.55V   │        │        │        │  │
│ └────────┴────────┴────────┴────────┘  │
└─────────────────────────────────────────┘
```

---

## 📋 Quick Reference

### **Status Indicators**
- 🟢 **Connected/Detected** - Sensor working
- 🔴 **Not Connected** - Sensor missing or no response
- 🟡 **Detecting...** - In progress
- ⚠️ **Warning** - Unexpected response

### **Console Emojis**
- 🔄 - Starting process
- ▶ - Sending command
- ✓ - Success
- ✗ - Failed
- ⚠ - Warning
- << - Receiving data
- 📊 - Statistics
- 📡 - Serial communication
- 📋 - Help text

### **Command Codes**
- `LIMIT_CHECK` - Read limit switch
- `OBSTACLE_CHECK` - Read obstacle sensor
- `ULTRASONIC_READ` - Read distance
- `PRESSURE_READ` - Read pressure sensor

---

## 🎓 Common Questions

**Q: Why does it take ~9 seconds?**  
A: Each sensor has 2-second timeout + 300ms delay between probes for Arduino processing

**Q: Can I speed it up?**  
A: Reduce timeout in code, but risks missing slower sensors

**Q: What if only one sensor fails?**  
A: It will show "Connected: 3/4" and you can identify which one failed in console

**Q: Do I need all 4 sensors?**  
A: No, system works with any combination. Fix/add as needed.

**Q: Can I test individual sensors?**  
A: Yes, use Arduino Control panel to send commands manually

---

**You're all set! Happy sensor detecting! 🚀✅**
