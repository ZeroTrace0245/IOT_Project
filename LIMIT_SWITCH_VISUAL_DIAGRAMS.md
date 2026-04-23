# Limit Switch Testing - Visual Diagrams & Flow Charts

## 1. Hardware Wiring Diagram

```
┌──────────────────────────────────────────────────────┐
│                  LIMIT SWITCH WIRING                 │
└──────────────────────────────────────────────────────┘

					 Limit Switch
						 │
			┌────────────┴────────────┐
			│                         │
		 Pin 1                     Pin 2
		 (Signal)                 (Ground)
			│                         │
			│                         │
			▼                         ▼
	   ┌─────────┐             ┌─────────┐
	   │         │             │         │
	   │ Arduino │             │ Arduino │
	   │  Pin 4  │             │   GND   │
	   │         │             │         │
	   └─────────┘             └─────────┘
```

---

## 2. State Diagram

```
┌─────────────────────────────────────────────────────┐
│         LIMIT SWITCH STATE MACHINE                  │
└─────────────────────────────────────────────────────┘

			  UNPRESSED STATE                 PRESSED STATE
			  ─────────────                   ────────────
					▲                              ▲
					│                              │
		🟢 HIGH     │                    🔴 LOW    │
		5V          │                    0V        │
		OPEN        │                    CLOSED    │
					│                              │
		No Contact  │                  Contact Made
			  ┌─────┴──────────────────────┬──────┘
			  │                            │
			  │   User Presses Switch      │
			  └────────────────────────────┘


TRANSITION TIMING:

		Open ──[User Press]──▶ Closed
		HIGH ─────────────────▶ LOW        [Instant: <10ms]
		🟢 ─────────────────────🔴


		Closed ──[User Release]──▶ Open
		LOW ─────────────────────▶ HIGH    [Instant: <10ms]
		🔴 ────────────────────────🟢
```

---

## 3. WinUI Monitor Display Layout

```
┌────────────────────────────────────────────────────────────┐
│  7. LIMIT SWITCH MONITOR - PIN 4                          │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  🔔 Limit Switch Monitor - Pin 4                          │
│  ──────────────────────────────────────────────────────────│
│                                                            │
│  ▶ Start Monitor  ⏹ Stop Monitor  Clear                  │
│                                                            │
│  ┌──────────────────────────┬──────────────────────────┐  │
│  │   Current State:         │   Presses Detected: 5   │  │
│  │                          │   Last Event:           │  │
│  │  ┌────────────────────┐  │   Released 14:32:15.842 │  │
│  │  │  🟢 OPEN (HIGH)   │  │                          │  │
│  │  └────────────────────┘  │                          │  │
│  └──────────────────────────┴──────────────────────────┘  │
│                                                            │
│  Event Log:                                               │
│  ┌──────────────────────────────────────────────────────┐ │
│  │ [14:32:15.842] ▶ Monitoring started...              │ │
│  │ [14:32:16.125] 🔴 PRESSED - Press #1               │ │
│  │ [14:32:16.342] 🟢 RELEASED                          │ │
│  │ [14:32:17.105] 🔴 PRESSED - Press #2               │ │
│  │ [14:32:17.301] 🟢 RELEASED                          │ │
│  │ [14:32:18.082] 🔴 PRESSED - Press #3               │ │
│  │ [14:32:18.298] 🟢 RELEASED                          │ │
│  │ [14:32:19.456] 🔴 PRESSED - Press #4               │ │
│  │ [14:32:19.652] 🟢 RELEASED                          │ │
│  │ [14:32:20.734] 🔴 PRESSED - Press #5               │ │
│  │ [14:32:20.941] 🟢 RELEASED                          │ │
│  └──────────────────────────────────────────────────────┘ │
│                                                            │
│  📋 Instructions:                                          │
│  1. Click 'Start Monitor' to begin monitoring             │
│  2. Press the limit switch connected to Pin 4             │
│  3. Watch the state change from HIGH → LOW when pressed   │
│  4. Release the switch to return to HIGH                  │
│  5. Check 'Presses Detected' counter for activity        │
│                                                            │
│  🔌 Wiring:                                                │
│  Limit Switch Pin 1 → Arduino Pin 4                       │
│  Limit Switch Pin 2 → Arduino Ground (GND)               │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

---

## 4. Testing Flow Chart

```
┌──────────────────────────────────────────────────────────┐
│              LIMIT SWITCH TESTING WORKFLOW               │
└──────────────────────────────────────────────────────────┘

						START
						  │
						  ▼
				  ┌────────────────┐
				  │ SETUP PHASE    │
				  │ (2-3 minutes)  │
				  └────────┬───────┘
						   │
		 ┌─────────────────┼─────────────────┐
		 │                 │                 │
		 ▼                 ▼                 ▼
	Wire Switch      Connect       Select COM
	to Pin 4         Arduino       Port in App
		 │                 │                 │
		 └─────────────────┼─────────────────┘
						   │
						   ▼
				  ┌────────────────┐
				  │ Launch App     │
				  └────────┬───────┘
						   │
						   ▼
				  ┌────────────────────────────┐
				  │ Choose Test Method         │
				  └────┬──────────────┬────────┘
					   │              │
		  ┌────────────┴──┐   ┌──────┴──────────┐
		  │               │   │                 │
		  ▼               ▼   ▼                 ▼
	 ┌─────────┐   ┌──────────────┐   ┌─────────────┐
	 │  Test 4 │   │  Test 7      │   │  Arduino    │
	 │  Quick  │   │  Monitor ⭐  │   │  IDE Serial │
	 │  (1 min)│   │  (3 min)     │   │             │
	 └────┬────┘   └──────┬───────┘   └─────┬───────┘
		  │               │                 │
		  ▼               ▼                 ▼
	 Run Test        Start Monitor      Open Serial
	 Once            Monitor Live        Monitor
	 Pass/Fail       Real-Time           Continuous
		  │               │                 │
		  └───────────────┼─────────────────┘
						  │
						  ▼
				 ┌────────────────────┐
				 │ VALIDATION PHASE   │
				 │ (Check Results)    │
				 └────────┬───────────┘
						  │
		 ┌────────────────┼────────────────┐
		 │                │                │
		 ▼                ▼                ▼
	State HIGH?    Counter OK?       No Flicker?
	Initial: 🟢    +1 per press      Stable state
		 │                │                │
		 └────────────────┼────────────────┘
						  │
				  ┌───────▼────────┐
				  │ Press 5-10     │
				  │ More Times     │
				  └───────┬────────┘
						  │
				  ┌───────▼────────┐
				  │ All Checks OK? │
				  └───┬────────┬───┘
					  │        │
				   YES│        │NO
					  ▼        ▼
					✅        ❌
				 SUCCESS   TROUBLESHOOT
					│          │
					│      Check Wiring
					│      Test Cables
					│      Try Again
					│          │
					└────┬─────┘
						 │
						 ▼
				  ┌──────────────┐
				  │  COMPLETE    │
				  │  TESTING     │
				  └──────────────┘
```

---

## 5. State Transition Diagram with Colors

```
┌────────────────────────────────────────────────────────────────┐
│          REAL-TIME STATE WITH COLOR INDICATORS                 │
└────────────────────────────────────────────────────────────────┘

RESTING STATE:                   ACTIVATED STATE:
┌──────────────────┐            ┌──────────────────┐
│                  │            │                  │
│  🟢 OPEN (HIGH) │  ◄────►   │ 🔴 PRESSED (LOW) │
│                  │            │                  │
│  Color: GREEN    │            │  Color: RED      │
│  Voltage: 5V     │            │  Voltage: 0V     │
│  Time Delay: 0ms │            │  Time Delay: 0ms │
│                  │            │                  │
│  Press Count:    │            │  Press Count:    │
│  (no change)     │            │  (increments)    │
│                  │            │                  │
└──────────────────┘            └──────────────────┘


TRANSITION ANIMATION:

	🟢 (Green) HIGH
		 │
		 │ [User Presses]
		 │ <10ms delay
		 ▼
	🔴 (Red) LOW
		 │
		 │ [User Releases]
		 │ <10ms delay
		 ▼
	🟢 (Green) HIGH
```

---

## 6. Event Log Sequence

```
┌────────────────────────────────────────────────────────────────┐
│              TYPICAL EVENT LOG SEQUENCE                        │
└────────────────────────────────────────────────────────────────┘

Timeline          Event                    State    Counter
────────          ─────                    ─────    ───────
14:32:15.842  ▶   Monitoring started      ─────    0
14:32:16.125  🔴  Press detected          LOW      1
14:32:16.342  🟢  Released                HIGH     1
14:32:17.105  🔴  Press detected          LOW      2
14:32:17.301  🟢  Released                HIGH     2
14:32:18.082  🔴  Press detected          LOW      3
14:32:18.298  🟢  Released                HIGH     3
14:32:19.456  🔴  Press detected          LOW      4
14:32:19.652  🟢  Released                HIGH     4
14:32:20.734  🔴  Press detected          LOW      5
14:32:20.941  🟢  Released                HIGH     5
14:32:45.123  ⏹   Monitoring stopped      ─────    5


STATISTICS:
Total Presses: 5
Total Releases: 5
Last Event: Release at 14:32:20.941
Duration: 29.281 seconds
Press Rate: 10.2 presses/minute
```

---

## 7. Troubleshooting Decision Tree

```
┌────────────────────────────────────────────────────────────┐
│        TROUBLESHOOTING DECISION TREE                       │
└────────────────────────────────────────────────────────────┘

					PROBLEM?
						│
		 ┌──────────────┼──────────────┐
		 │              │              │
		 ▼              ▼              ▼
	No State      Stays HIGH    Stays LOW
	Change        Always        Always
		 │              │              │
		 ▼              ▼              ▼
	Check:         Check:         Check:
	- Wiring      - Wire tight    - Stuck
	- COM port    - Test button   - Contact
	- Connection  - Multimeter    - Clean
		 │              │              │
		 ▼              ▼              ▼
	Reseat       Wiggle Wires   Press Hard
	All Wires    During Test    Vigorously
		 │              │              │
		 └──────────────┼──────────────┘
						│
				  Works Now?
						│
		 ┌──────────────┼──────────────┐
		 │              │              │
		 NO             │             YES
		 │              │              │
		 ▼              ▼              ▼
	Try Arduino   Continue    ✅ Success
	IDE Serial    Testing     All Good
	Monitor       ...
		 │
	Still No?
		 │
		 ▼
	Hardware
	Issue
	Likely
```

---

## 8. Serial Communication Diagram

```
┌──────────────────────────────────────────────────────────┐
│      SERIAL DATA FLOW BETWEEN ARDUINO & APP               │
└──────────────────────────────────────────────────────────┘

ARDUINO SIDE:
┌─────────────────────────────────┐
│   Pin 4 (Limit Switch)          │
│   ├─ Reads digital state        │
│   ├─ HIGH (5V) = not pressed    │
│   └─ LOW (0V) = pressed         │
│            │                     │
│            ▼                     │
│   Sends via Serial              │
│   ├─ SWITCH_STATE=HIGH          │
│   └─ SWITCH_STATE=LOW           │
│            │                     │
└────────────┼─────────────────────┘
			 │
		 USB Cable
		(9600 baud)
			 │
		┌────▼─────────────────────┐
		│                          │
		│  WinUI Application       │
		│  ├─ Receives data        │
		│  ├─ Parses state         │
		│  ├─ Updates UI           │
		│  ├─ Logs events          │
		│  └─ Increments counter   │
		│                          │
		└──────────────────────────┘


DATA FORMAT:
┌─────────────────────────────────┐
│  SWITCH_STATE=HIGH              │
│  SWITCH_STATE=LOW               │
└─────────────────────────────────┘

FREQUENCY:
├─ Polling: Every 50ms
├─ Updates: Real-time to UI
└─ Log Rate: Per state change
```

---

## 9. Success Criteria Checklist

```
┌──────────────────────────────────────────────────────────┐
│           SUCCESSFUL TEST CHECKLIST                      │
└──────────────────────────────────────────────────────────┘

INITIAL STATE:
  ☐ Display shows: 🟢 OPEN (HIGH)
  ☐ Color is GREEN
  ☐ Counter shows: 0
  ☐ Last Event shows: Ready

PRESSING SWITCH:
  ☐ Display changes to: 🔴 PRESSED (LOW)
  ☐ Color changes to RED
  ☐ Counter increments to: 1
  ☐ Event logged with timestamp
  ☐ Change happens INSTANTLY

RELEASING SWITCH:
  ☐ Display changes back to: 🟢 OPEN (HIGH)
  ☐ Color changes back to GREEN
  ☐ Counter STAYS at: 1
  ☐ Event logged: RELEASED
  ☐ Change happens INSTANTLY

REPEATED PRESSES:
  ☐ Press 5-10 more times
  ☐ Counter goes: 1, 2, 3, 4, 5...
  ☐ Each press/release logged
  ☐ NO FLICKERING observed
  ☐ State ALWAYS stable

FINAL VALIDATION:
  ☐ All 12 items above checked
  ☐ No errors in event log
  ☐ No connection warnings
  ☐ Timing all instant
  ☐ No repeated states


RESULT:  ✅ LIMIT SWITCH WORKING PERFECTLY
```

---

## 10. Hardware vs Software Timing

```
┌────────────────────────────────────────────────────────────┐
│           TIMING ANALYSIS                                 │
└────────────────────────────────────────────────────────────┘

HARDWARE RESPONSE:
┌─────────────────────────────────────────┐
│ User presses button                     │
│ │                                       │
│ └──▶ Mechanical switch closes           │
│      (~50ms - mechanical delay)         │
│      │                                  │
│      └──▶ Pin 4 voltage drops to GND    │
│           (essentially immediate)       │
│           (< 1ms)                       │
│           │                             │
│           └──▶ Arduino detects change   │
│                (immediate read)         │
│                │                        │
│                └──▶ Firmware processes  │
│                     (few microseconds)  │
│                     │                   │
│                     └──▶ Sends via Serial
│                          (small delay)  │
│                          │              │
│                          └──▶ App receives
│                               (instant)
│                               │
│                               └──▶ UI Updates
│                                    (< 100ms)
│
│ TOTAL: ~150-200ms from press to display
└─────────────────────────────────────────┘


EXPECTED TIMES:
├─ Physical contact: Immediate
├─ Electrical signal: < 1ms
├─ Serial transmission: 1-5ms
├─ UI update: 10-50ms
└─ Total visual: 50-100ms
```

---

## Visual State Machine

```
┌────────────────────────────────────────────────────────┐
│          VISUAL STATE TRANSITION                       │
└────────────────────────────────────────────────────────┘

		 ╔═══════════════════╗
		 ║                   ║
		 ║   🟢 OPEN (HIGH)  ║  ◄───── DEFAULT STATE
		 ║                   ║
		 ║  GREEN indicator  ║
		 ║  Voltage: 5V      ║
		 ║                   ║
		 ╚═════════╤═════════╝
				   │
		  Press Switch ↓
				   │
		 ╔═════════▼═════════╗
		 ║                   ║
		 ║  🔴PRESSED (LOW)  ║  ◄───── ACTIVE STATE
		 ║                   ║
		 ║  RED indicator    ║
		 ║  Voltage: 0V      ║
		 ║                   ║
		 ╚═════════╤═════════╝
				   │
		 Release Switch ↑
				   │
		 ╔═════════▼═════════╗
		 ║                   ║
		 ║   🟢 OPEN (HIGH)  ║  ◄───── BACK TO DEFAULT
		 ║                   ║
		 ║  GREEN indicator  ║
		 ║  Voltage: 5V      ║
		 ║                   ║
		 ╚═══════════════════╝
```

---

**Visual Diagrams Complete**  
All testing workflows, state transitions, and UI layouts illustrated above.
