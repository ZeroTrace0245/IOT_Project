# Sensor Detection System - Visual Diagrams

## 1. System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    IOT APP - Sensor Detection                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                     UI Layer                             │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │                                                           │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │   │
│  │  │  Buttons     │  │  Sensor      │  │  Serial      │   │   │
│  │  │              │  │  Cards UI    │  │  Console     │   │   │
│  │  │ •Start       │  │              │  │              │   │   │
│  │  │ •Validate    │  │ • Limit      │  │ • Commands   │   │   │
│  │  │ •Reset       │  │ • Obstacle   │  │ • Responses  │   │   │
│  │  │ •Help        │  │ • Ultrasonic │  │ • Timestamps │   │   │
│  │  │ • Clear      │  │ • Pressure   │  │              │   │   │
│  │  └──────────────┘  └──────────────┘  └──────────────┘   │   │
│  │                                                           │   │
│  └──────────────────────────────────────────────────────────┘   │
│                           ▲                                       │
│                           │ DispatcherQueue                      │
│                           │ (Thread-safe updates)                │
│                           ▼                                       │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                  Business Logic Layer                    │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │                                                           │   │
│  │  StartSensorDetection_Click()                            │   │
│  │         │                                                 │   │
│  │         └─► Background Task                              │   │
│  │             (Async Detection)                            │   │
│  │                │                                          │   │
│  │                ├─► DetectSensorAsync("LimitSwitch",...)  │   │
│  │                │        └─ Send LIMIT_CHECK              │   │
│  │                │        └─ Wait for LIMIT_STATE=X        │   │
│  │                │        └─ Validate & Update UI          │   │
│  │                │                                          │   │
│  │                ├─► DetectSensorAsync("Obstacle",...)     │   │
│  │                │        └─ Send OBSTACLE_CHECK           │   │
│  │                │        └─ Wait for OBSTACLE_STATE=X     │   │
│  │                │        └─ Validate & Update UI          │   │
│  │                │                                          │   │
│  │                ├─► DetectSensorAsync("Ultrasonic",...)   │   │
│  │                │        └─ Send ULTRASONIC_READ          │   │
│  │                │        └─ Wait for ULTRASONIC_DISTANCE= │   │
│  │                │        └─ Validate & Update UI          │   │
│  │                │                                          │   │
│  │                └─► DetectSensorAsync("Pressure",...)     │   │
│  │                         └─ Send PRESSURE_READ            │   │
│  │                         └─ Wait for PRESSURE_RAW=        │   │
│  │                         └─ Validate & Update UI          │   │
│  │                                                           │   │
│  └──────────────────────────────────────────────────────────┘   │
│                           ▲                                       │
│                           │ WriteToSerial()                      │
│                           │ Response from SerialPort_DataReceived│
│                           ▼                                       │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                   Serial Layer                          │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │                                                           │   │
│  │  SerialPort.Open()                                       │   │
│  │  SerialPort.DataReceived                                 │   │
│  │  SerialPort_DataReceived()                               │   │
│  │      │                                                    │   │
│  │      └─ If waiting for sensor response:                 │   │
│  │         • Store response in _lastSensorResponse          │   │
│  │         • Signal ManualResetEvent                        │   │
│  │         • Add to _sensorConsoleCollection                │   │
│  │                                                           │   │
│  └──────────────────────────────────────────────────────────┘   │
│                           ▲                                       │
│                           │ Serial Communication                 │
│                           ▼                                       │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                    Arduino Board                         │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │                                                           │   │
│  │  Receives: LIMIT_CHECK                                   │   │
│  │  Sends:    LIMIT_STATE=1                                 │   │
│  │                                                           │   │
│  │  Receives: OBSTACLE_CHECK                                │   │
│  │  Sends:    OBSTACLE_STATE=0                              │   │
│  │                                                           │   │
│  │  Receives: ULTRASONIC_READ                               │   │
│  │  Sends:    ULTRASONIC_DISTANCE=45                        │   │
│  │                                                           │   │
│  │  Receives: PRESSURE_READ                                 │   │
│  │  Sends:    PRESSURE_RAW=523                              │   │
│  │                                                           │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │   │
│  │  │ Limit Switch │  │  Obstacle    │  │ Ultrasonic   │   │   │
│  │  │  (Pin 4)     │  │  (Pin 3)     │  │ (Pins 1/2)   │   │   │
│  │  └──────────────┘  └──────────────┘  └──────────────┘   │   │
│  │                                                           │   │
│  │  ┌──────────────┐                                        │   │
│  │  │  Pressure    │                                        │   │
│  │  │  (Pin A0)    │                                        │   │
│  │  └──────────────┘                                        │   │
│  │                                                           │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Detection Flow Diagram

```
START
  │
  ▼
User clicks "Start Handshake"
  │
  ▼
Verify Arduino Connected?
  ├─ NO  ──► Show Error ──► END
  │
  └─ YES ──► Clear Console
			 └─► Start Background Task
				  │
				  ▼
				For each sensor:
				  │
				  ├─► LIMIT_CHECK
				  │   Send: "LIMIT_CHECK"
				  │   Wait: 2 seconds for response
				  │   Response Received?
				  │   ├─ YES: Validate prefix "LIMIT_STATE="
				  │   │       ├─ VALID: Mark as detected ✓
				  │   │       │          Update UI card
				  │   │       │          Log to console
				  │   │       └─ INVALID: Log error ✗
				  │   └─ NO: Timeout ──► Mark as not detected ✗
				  │       Log "No response"
				  │       Continue to next sensor (300ms delay)
				  │
				  ├─► OBSTACLE_CHECK
				  │   [Same flow as above]
				  │
				  ├─► ULTRASONIC_READ
				  │   [Same flow as above]
				  │
				  └─► PRESSURE_READ
					  [Same flow as above]
					  │
					  ▼
				Complete
				  │
				  ▼
				Calculate Duration = End Time - Start Time
				  │
				  ▼
				Update Summary:
				  • Connected Count (0-4)
				  • Status: "Complete"
				  • Handshake Time: XXXms
				  • Last Check: HH:mm:ss
				  │
				  ▼
				Show Results in UI
				  │
				  ▼
				END
```

---

## 3. Response Handling Sequence Diagram

```
					APP                        SERIAL             ARDUINO
					│                           │                    │
					│─────────────────────────► │ WriteToSerial()    │
					│   Send LIMIT_CHECK        │                    │
					│                           │─────────────────► │
					│                           │   "LIMIT_CHECK"    │
					│                           │                    │
					│   SetWaitHandle.Reset()   │     Process        │
					│   _waitingForSensorResponse=true                │
					│                           │                    │
					│   WaitOne(2000ms)         │                    │
					│   ↓ WAITING...            │ <─────────────────│
					│                           │  "LIMIT_STATE=1"   │
					│                           │                    │
					│   SerialPort_DataReceived │ ◄─────────────────│
					│◄──────────────────────────│                    │
					│                           │                    │
					│   _lastSensorResponse =   │                    │
					│   "LIMIT_STATE=1"         │                    │
					│   SetWaitHandle.Set()     │                    │
					│   AddSensorConsoleLog()   │                    │
					│                           │                    │
					│   WaitOne() returns       │                    │
					│                           │                    │
					│   Validate response:      │                    │
					│   "LIMIT_STATE=".Equals(.StartsWith())         │
					│                           │                    │
					│   Extract value: "1"      │                    │
					│   _sensorStatus["LimitSwitch"] = true          │
					│   _sensorReadings["LimitSwitch"] = "1"         │
					│                           │                    │
					│   UpdateSensorCardUI()    │                    │
					│   └─ Update UI with value │                    │
					│   └─ Change status to ✓   │                    │
					│                           │                    │
					│   Delay 300ms             │                    │
					│                           │                    │
					│   Next sensor...          │                    │
					│                           │                    │
					▼                           ▼                    ▼
```

---

## 4. Data Flow Diagram

```
┌─────────────────────────────────────────────────────────┐
│           ObservableCollection & Dictionaries           │
└─────────────────────────────────────────────────────────┘
		 ▲                    ▲                ▲
		 │                    │                │
		 │                    │                │
		 │                    │                │
	┌────┴─────┐          ┌──┴───┐      ┌────┴──────┐
	│ Console  │          │Status│      │ Readings  │
	│Collection│          │Dict  │      │Dictionary │
	└────┬─────┘          └──┬───┘      └────┬──────┘
		 │                    │              │
		 │ AddSensorConsoleLog│ DetectSensor │ UpdateSensorCard
		 │                    │ Async        │
		 ▼                    ▼              ▼
	┌────────────────────────────────────────────┐
	│        Sensor Detection Methods            │
	├────────────────────────────────────────────┤
	│  • StartSensorDetection_Click()            │
	│  • DetectSensorAsync()                     │
	│  • UpdateSensorCardUI()                    │
	│  • ValidateSensors_Click()                 │
	│  • ResetDetection_Click()                  │
	│  • SensorHelp_Click()                      │
	└────────────────────────────────────────────┘
		 ▲              ▲              ▲
		 │              │              │
		 │              │              │
	  EVENTS         UI BINDINGS    LOGS
		 │              │              │
		 ▼              ▼              ▼
	┌────────────────────────────────────────────┐
	│             UI Components                  │
	├────────────────────────────────────────────┤
	│  • Buttons: Start, Validate, Reset, Help  │
	│  • Cards: Sensor information display      │
	│  • Console: Real-time log display         │
	│  • Stats: Connected count, time, status   │
	└────────────────────────────────────────────┘
```

---

## 5. Timing Diagram

```
Time (seconds)
0          1          2          3          4          5          6          7          8          9
│          │          │          │          │          │          │          │          │          │
├──────────┼──────────┼──────────┼──────────┼──────────┼──────────┼──────────┼──────────┼──────────┤

[START HANDSHAKE]
│
├─► Sensor 1: LIMIT_CHECK
│   Send ──────── Wait (2 sec) ──── Response ─ Process ─ Done
│   0             2                  X         X+0.1     X+0.1
│
│   Delay 300ms
│
├─► Sensor 2: OBSTACLE_CHECK
│   Send ──────── Wait (2 sec) ──── Response ─ Process ─ Done
│   0.4           2.4                Y         Y+0.1     Y+0.1
│
│   Delay 300ms
│
├─► Sensor 3: ULTRASONIC_READ
│   Send ──────── Wait (2 sec) ──── Response ─ Process ─ Done
│   0.8           2.8                Z         Z+0.1     Z+0.1
│
│   Delay 300ms
│
└─► Sensor 4: PRESSURE_READ
	Send ──────── Wait (2 sec) ──── Response ─ Process ─ Done
	1.2           3.2                W         W+0.1     W+0.1
															  │
															  ▼
													[HANDSHAKE COMPLETE]
													Total Time: ~3.4 - 9.2s

LEGEND:
  Send     = Arduino command sent
  Wait     = Waiting for response (up to 2 seconds)
  Response = Response received from Arduino
  Process  = Validate & Update UI
  Delay    = 300ms delay before next sensor

WORST CASE (all timeouts):
  4 sensors × (2s timeout + 0.3s delay) = 9.2 seconds

BEST CASE (instant responses):
  4 sensors × (0.1s response + 0.3s delay) = 1.6 seconds

TYPICAL CASE (200ms responses):
  4 sensors × (0.2s response + 0.3s delay) = 2.0 seconds
```

---

## 6. State Machine Diagram

```
					┌─────────────┐
					│   IDLE      │
					└──────┬──────┘
						   │
					User clicks "Start"
						   │
						   ▼
					┌─────────────────────┐
					│ CHECKING_CONNECTION │
					└──────┬──────┬───────┘
						   │      │
					 Connected   Not Connected
						   │           │
						   ▼           ▼
					┌────────────┐  ERROR
					│ DETECTING  │   │
					└────┬───┬──┘    │
						 │   │       │
				   [For each sensor] │
						 │   │       │
					┌────▼───▼───┐   │
					│ PROBING    │   │
					└────┬───┬──┘    │
						 │   │       │
				  [Wait 2sec timeout] │
						 │   │       │
					┌────▼───▼──────┐ │
					│ VALIDATING    │ │
					└────┬───┬──┬───┘ │
					Valid  Invalid Timeout
					├─DETECTED ─┤ ├─NOT_DETECTED ─┤
						 │           │
					┌────▼───────────▼──┐
					│ UI_UPDATING       │
					└────┬──────────┬──┘
						 │          │
					[300ms delay]   │
						 │          │
					More sensors?   │
					├─ YES ─ Go to PROBING
					└─ NO ──┘
						   │
						   ▼
					┌──────────────┐
					│ CALCULATING  │
					│ RESULTS      │
					└──────┬───────┘
						   │
						   ▼
					┌──────────────────┐
					│ DISPLAYING       │
					│ RESULTS          │
					└──────┬──────┬────┘
						   │      │
					  More tests? │
						│         │
						▼         │
				   IDLE ◄─────────┘
					│
					▼
		[Ready for next handshake]
```

---

## 7. Collection Data Structure Diagram

```
┌─────────────────────────────────────────────────┐
│  _sensorConsoleCollection                       │
│  ObservableCollection<string>                   │
├─────────────────────────────────────────────────┤
│  [00] "[15:32:45] ✓ Handshake Complete"         │
│  [01] "[15:32:45] 📊 Connected: 4/4 sensors"   │
│  [02] "[15:32:45] ✓ Pressure DETECTED - 523"   │
│  [03] "[15:32:45] << Response: PRESSURE_RAW=523│
│  [04] "[15:32:44] ▶ Pressure: Sending..."      │
│  [05] "[15:32:44] << Response: ULTRASONIC_DIST │
│  ...                                            │
│  [MAX] (200 entries max)                       │
└─────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────┐
│  _sensorStatus                                  │
│  Dictionary<string, bool>                       │
├─────────────────────────────────────────────────┤
│  "LimitSwitch"  = true                          │
│  "Obstacle"     = true                          │
│  "Ultrasonic"   = true                          │
│  "Pressure"     = true                          │
│  "MotorPWM"     = false                         │
│  "Stepper"      = false                         │
└─────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────┐
│  _sensorReadings                                │
│  Dictionary<string, string>                     │
├─────────────────────────────────────────────────┤
│  "LimitSwitch"  = "1"                           │
│  "Obstacle"     = "0"                           │
│  "Ultrasonic"   = "45"                          │
│  "Pressure"     = "523"                         │
└─────────────────────────────────────────────────┘
```

---

## 8. Error Handling Tree

```
START Detection
   │
   ├─ Arduino Not Connected?
   │  └─ YES ──► Show Error Dialog
   │            Log: "ERROR: Arduino not connected!"
   │            Return
   │
   └─ Arduino Connected?
	  └─ YES
		 │
		 ├─ For Each Sensor:
		 │  │
		 │  ├─ Send Command
		 │  │  │
		 │  │  └─ Wait for Response
		 │  │     │
		 │  │     ├─ Timeout (2 seconds)?
		 │  │     │  └─ YES ──► Log: "✗ No response (timeout)"
		 │  │     │             Mark sensor: NOT_DETECTED
		 │  │     │
		 │  │     ├─ Response Received?
		 │  │     │  └─ YES
		 │  │     │     │
		 │  │     │     ├─ Validate Prefix?
		 │  │     │     │  │
		 │  │     │     │  ├─ VALID ──► Log: "✓ Detected"
		 │  │     │     │  │            Mark sensor: DETECTED
		 │  │     │     │  │            Extract & Store Value
		 │  │     │     │  │            Update UI Card
		 │  │     │     │  │
		 │  │     │     │  └─ INVALID ──► Log: "⚠ Unexpected response"
		 │  │     │     │                  Mark sensor: NOT_DETECTED
		 │  │     │     │                  Show expected format
		 │  │     │     │
		 │  │     │     └─ No Exception?
		 │  │     │        └─ Exception Caught ──► Log Error
		 │  │     │                                 Continue to next sensor
		 │  │     │
		 │  │     └─ No Response After Timeout?
		 │  │        └─ YES ──► Mark sensor: NOT_DETECTED
		 │  │
		 │  └─ Delay 300ms
		 │
		 └─ All Sensors Complete?
			└─ YES ──► Calculate Results
					   Update UI Summary
					   Display Success Message
```

---

These diagrams provide a complete visual understanding of:
- ✅ System architecture and layers
- ✅ Detection process flow
- ✅ Response handling sequence
- ✅ Data flow between components
- ✅ Timing characteristics
- ✅ State transitions
- ✅ Data structure organization
- ✅ Error handling paths

Use these to understand how the sensor detection system works internally!
