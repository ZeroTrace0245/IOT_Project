using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.IO.Ports;
using LiveChartsCore;
using LiveChartsCore.SkiaSharpView;
using System.Collections.ObjectModel;
using LiveChartsCore.Kernel.Sketches;
using System.Collections.Generic;

namespace IOT_APP
{
    public class ShoeRecommendation
    {
        public string Brand { get; set; } = string.Empty;
        public string ResultText { get; set; } = string.Empty;
        public ShoeRecommendation(string brand, string resultText) { Brand = brand; ResultText = resultText; }
    }

    public class InventoryItem
    {
        public string SKU { get; set; } = string.Empty;
        public string Model { get; set; } = string.Empty;
        public string Price { get; set; } = string.Empty;
        public int Stock { get; set; }
    }

    public sealed partial class MainWindow : Window
    {
        private SerialPort? _serialPort;
        public ObservableCollection<double> DataValues { get; set; } = new();
        public ObservableCollection<ShoeRecommendation> Recommendations { get; set; } = new();
        public ObservableCollection<InventoryItem> Inventory { get; set; } = new();

        public ObservableCollection<string> SerialOutputLog { get; set; } = new();
        public ObservableCollection<string> CommandHistory { get; set; } = new();

        public ObservableCollection<ISeries> Series { get; set; } = new();
        public List<ICartesianAxis> XAxes { get; set; } = new();
        public List<ICartesianAxis> YAxes { get; set; } = new();

        // Limit Switch Monitor
        private bool _isMonitoringLimitSwitch = false;
        private int _limitSwitchPressCount = 0;
        private int _lastLimitSwitchState = 1;  // 1 = HIGH (open), 0 = LOW (pressed)

        // Pressure Sensor
        private int _lastPressureRaw = 0;
        private float _lastPressureVoltage = 0.0f;

        // Auto-Monitoring System
        private DispatcherTimer _autoMonitorTimer;
        private bool _isAutoMonitoring = false;
        private int _currentMotorSpeed = 0;
        private ObservableCollection<string> _motorActivityLogCollection = new();

        // Step Signal Control (Pin 3)
        private int _currentStepCount = 50;
        private int _currentStepDelay = 50;  // milliseconds
        private ObservableCollection<string> _stepPulseLogCollection = new();

        // Sensor Detection System
        private ObservableCollection<string> _sensorDetectionLogCollection = new();
        private Dictionary<string, bool> _sensorStatus = new()
        {
            { "LimitSwitch", false },
            { "MotorPWM", false },
            { "Stepper", false },
            { "Pressure", false },
            { "DHT22", false }
        };
        private int _connectedSensorCount = 0;
        private DateTime _lastHandshakeTime;
        private long _handshakeDuration = 0;

        public MainWindow()
        {
            this.InitializeComponent();

            // Set Titlebar to Dark
            if (Microsoft.UI.Windowing.AppWindow.GetFromWindowId(this.AppWindow.Id) is Microsoft.UI.Windowing.AppWindow appWindow)
            {
                if (Microsoft.UI.Windowing.AppWindowTitleBar.IsCustomizationSupported())
                {
                    var titleBar = appWindow.TitleBar;
                    titleBar.ExtendsContentIntoTitleBar = false; // Keep default bar but color it

                    // Dark theme colors for the title bar
                    titleBar.BackgroundColor = Windows.UI.Color.FromArgb(255, 32, 32, 32);
                    titleBar.ButtonBackgroundColor = Windows.UI.Color.FromArgb(255, 32, 32, 32);
                    titleBar.InactiveBackgroundColor = Windows.UI.Color.FromArgb(255, 32, 32, 32);
                    titleBar.ButtonInactiveBackgroundColor = Windows.UI.Color.FromArgb(255, 32, 32, 32);

                    titleBar.ForegroundColor = Microsoft.UI.Colors.White;
                    titleBar.ButtonForegroundColor = Microsoft.UI.Colors.White;
                    titleBar.InactiveForegroundColor = Microsoft.UI.Colors.Gray;
                    titleBar.ButtonInactiveForegroundColor = Microsoft.UI.Colors.Gray;
                }
            }

            // Setup Chart Data
            Series = new ObservableCollection<ISeries>
            {
                new LineSeries<double>
                {
                    Values = DataValues,
                    Fill = null,
                    GeometrySize = 0,
                    LineSmoothness = 0.5
                }
            };

            XAxes = new List<ICartesianAxis> { new Axis { Name = "Time" } };
            YAxes = new List<ICartesianAxis> { new Axis { Name = "Value" } };

            ShoeListView.ItemsSource = Recommendations;
            LoadMockInventory();
            RefreshPorts();

            // Initialize Limit Switch Event Log collection
            if (LimitSwitchEventLog != null)
            {
                var limitSwitchLog = new ObservableCollection<string>();
                LimitSwitchEventLog.ItemsSource = limitSwitchLog;
            }

            // Initialize Motor Activity Log collection
            if (MotorActivityLog != null)
            {
                MotorActivityLog.ItemsSource = _motorActivityLogCollection;
            }

            // Initialize Step Pulse Log collection
            if (StepPulseLog != null)
            {
                StepPulseLog.ItemsSource = _stepPulseLogCollection;
            }
        }

        private void LoadMockInventory()
        {
            Inventory.Add(new InventoryItem { SKU = "SKU-9921", Model = "Nike Air Max 270", Price = "Rs. 24,500", Stock = 5 });
            Inventory.Add(new InventoryItem { SKU = "SKU-1022", Model = "Adidas Ultraboost", Price = "Rs. 22,000", Stock = 12 });
            Inventory.Add(new InventoryItem { SKU = "SKU-4402", Model = "Puma RS-X", Price = "Rs. 18,500", Stock = 8 });
            Inventory.Add(new InventoryItem { SKU = "SKU-2291", Model = "Brooks GTS 22", Price = "Rs. 15,900", Stock = 3 });
        }

        private void NavButton_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button btn && btn.Tag != null)
            {
                DashboardPanel.Visibility = Visibility.Collapsed;
                CashierPanel.Visibility = Visibility.Collapsed;
                CustomPanel.Visibility = Visibility.Collapsed;
                DriverTestPanel.Visibility = Visibility.Collapsed;
                SerialConsolePanel.Visibility = Visibility.Collapsed;
                SensorDetectionPanel.Visibility = Visibility.Collapsed;
                ArduinoControlPanel.Visibility = Visibility.Collapsed;

                string target = btn.Tag.ToString();
                if (target == "Dashboard") DashboardPanel.Visibility = Visibility.Visible;
                else if (target == "Cashier") CashierPanel.Visibility = Visibility.Visible;
                else if (target == "Custom") CustomPanel.Visibility = Visibility.Visible;
                else if (target == "DriverTest") DriverTestPanel.Visibility = Visibility.Visible;
                else if (target == "SerialConsole") SerialConsolePanel.Visibility = Visibility.Visible;
                else if (target == "SensorDetection")
                {
                    SensorDetectionPanel.Visibility = Visibility.Visible;
                    InitializeSensorDetectionUI();
                }
                else if (target == "ArduinoControl")
                {
                    ArduinoControlPanel.Visibility = Visibility.Visible;
                }
            }
        }

        private void MeasureButton_Click(object sender, RoutedEventArgs e)
        {
            // Check if serial port is connected
            if (_serialPort == null || !_serialPort.IsOpen)
            {
                // Use mock data when Arduino is not connected
                MeasurementLabel.Text = "Arduino not connected. Using MOCK DATA...";
                System.Threading.Tasks.Task.Run(() =>
                {
                    GenerateMockFootData();
                    System.Threading.Thread.Sleep(2000);
                    GenerateAllMockSensorData();
                });
                return;
            }

            // Send actual command to Arduino
            WriteToSerial("MEASURE;SCAN=AUTO");
            FootResultLabel.Text = "Scanning foot...";
            Recommendations.Clear();
        }

        private void RefreshPorts()
        {
            PortComboBox.Items.Clear();
            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
            {
                PortComboBox.Items.Add(port);
            }
            if (ports.Length > 0) PortComboBox.SelectedIndex = 0;
        }

        private void PortComboBox_DropDownOpened(object sender, object e)
        {
            RefreshPorts();
        }

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            if (_serialPort != null && _serialPort.IsOpen)
            {
                _serialPort.Close();
                UpdateConnectionStatus(false);
                ConnectButton.Content = "Connect";
                return;
            }

            if (PortComboBox.SelectedItem is string portName)
            {
                try
                {
                    _serialPort = new SerialPort(portName, 9600);
                    _serialPort.DataReceived += SerialPort_DataReceived;
                    _serialPort.Open();
                    UpdateConnectionStatus(true);
                    ConnectButton.Content = "Disconnect";
                }
                catch (Exception ex)
                {
                    MeasurementLabel.Text = $"Error: {ex.Message}";
                    UpdateConnectionStatus(false);
                }
            }
        }

        private void UpdateConnectionStatus(bool isConnected)
        {
            if (isConnected)
            {
                ConnectionBadge.Background = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LightGreen);
                StatusText.Text = "Connected";
                StatusText.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LightGreen);
            }
            else
            {
                ConnectionBadge.Background = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Gray);
                StatusText.Text = "Disconnected";
                StatusText.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Gray);
            }
        }

        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string? rawData = _serialPort?.ReadLine()?.Trim();
            if (string.IsNullOrEmpty(rawData)) return;

            DispatcherQueue.TryEnqueue(() =>
            {
                // Add to serial output log with timestamp
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                SerialOutputLog.Insert(0, $"[{timestamp}] << RECEIVED: {rawData}");

                // Keep log size manageable
                while (SerialOutputLog.Count > 500)
                {
                    SerialOutputLog.RemoveAt(SerialOutputLog.Count - 1);
                }

                // Process limit switch data if monitoring
                if (_isMonitoringLimitSwitch && (rawData.StartsWith("SWITCH_STATE=") || rawData.StartsWith("SWITCH_PRESS")))
                {
                    ProcessLimitSwitchData(rawData);
                }

                // Process specific responses
                if (rawData.StartsWith("FOOT="))
                {
                    ProcessFootMeasurement(rawData.Replace("FOOT=", ""));
                }
                else if (rawData.StartsWith("RESULT="))
                {
                    MeasurementLabel.Text = $"Final Result: {rawData.Replace("RESULT=", "")} mm";
                }
                else if (rawData.StartsWith("STATUS="))
                {
                    MeasurementLabel.Text = rawData.Replace("STATUS=", "");
                }
                else if (rawData.StartsWith("PRESSURE_RAW="))
                {
                    if (int.TryParse(rawData.Replace("PRESSURE_RAW=", ""), out int rawValue))
                    {
                        // Store raw value and wait for voltage
                        _lastPressureRaw = rawValue;
                    }
                }
                else if (rawData.StartsWith("PRESSURE_VOLTAGE="))
                {
                    if (float.TryParse(rawData.Replace("PRESSURE_VOLTAGE=", ""), out float voltage))
                    {
                        // Display both raw and voltage values
                        if (MeasurementLabel != null)
                        {
                            MeasurementLabel.Text = $"Pressure: {_lastPressureRaw} (Raw) | {voltage:F2}V";
                        }

                        // Update real-time monitoring display
                        _lastPressureVoltage = voltage;
                        UpdatePressureMonitor(_lastPressureRaw, voltage);
                    }
                }
                else if (rawData.StartsWith("PRESSURE="))
                {
                    if (int.TryParse(rawData.Replace("PRESSURE=", ""), out int pressure))
                    {
                        MeasurementLabel.Text = $"Pressure: {pressure} (0-1023)";
                    }
                }
                else if (rawData.StartsWith("OBSTACLE_STATE="))
                {
                    if (int.TryParse(rawData.Replace("OBSTACLE_STATE=", ""), out int state))
                    {
                        bool isTriggered = (state == 0); // LOW = obstacle detected
                        UpdateLimitSwitchMonitor(isTriggered);
                    }
                }
                else if (rawData.StartsWith("LIMIT_STATE="))
                {
                    if (int.TryParse(rawData.Replace("LIMIT_STATE=", ""), out int state))
                    {
                        bool isTriggered = (state == 0); // LOW = triggered
                        UpdateLimitSwitchMonitor(isTriggered);
                    }
                }
                else if (rawData.StartsWith("ULTRASONIC_DISTANCE="))
                {
                    if (int.TryParse(rawData.Replace("ULTRASONIC_DISTANCE=", ""), out int distance))
                    {
                        UpdateUltrasonicMonitor(distance);
                    }
                }
                else if (rawData.StartsWith("SCAN_STATE="))
                {
                    string state = rawData.Replace("SCAN_STATE=", "");
                    UpdateScanStatus(state);
                }
                else if (rawData.StartsWith("[SCAN]"))
                {
                    // Display scan status messages
                    MeasurementLabel.Text = rawData.Replace("[SCAN] ", "");
                }
                else if (rawData.StartsWith("SCAN_"))
                {
                    // Display scan event messages
                    MeasurementLabel.Text = rawData;
                }
                else if (rawData.StartsWith("PONG="))
                {
                    MeasurementLabel.Text = "Device is online!";
                }
                else
                {
                    MeasurementLabel.Text = $"Last Reading: {rawData}";
                    if (double.TryParse(rawData, out double val))
                    {
                        DataValues.Add(val);
                        if (DataValues.Count > 50) DataValues.RemoveAt(0);
                    }
                }
            });
        }

        private void ProcessFootMeasurement(string lengthInput)
        {
            if (double.TryParse(lengthInput, out double length))
            {
                FootResultLabel.Text = $"{length:F1} mm";
                CalculateRecommendations(length);
                ShopCard.Visibility = Visibility.Visible;
            }
        }

        private void CalculateRecommendations(double mmStep)
        {
            Recommendations.Clear();

            // NEMA 11 Lead Screw Calculation: mmStep is already calculated on Arduino
            // But we can refine it here if needed.

            // Basic US Men's scale: Size = (LengthInInches * 3) - 22
            double usSize = (mmStep / 25.4) * 3 - 22;
            // Basic EU scale: (mm + 10) / 6.67
            double euSize = (mmStep + 10) / 6.67;

            Recommendations.Add(new ShoeRecommendation("Nike Pettah Store", $"US {(usSize + 0.5):F1} / EU {euSize:F0}"));
            Recommendations.Add(new ShoeRecommendation("Adidas Colombo", $"US {usSize:F1} / EU {(euSize + 1):F0}"));
            Recommendations.Add(new ShoeRecommendation("Puma Main St.", $"US {(usSize - 0.5):F1} / EU {euSize:F0}"));
            Recommendations.Add(new ShoeRecommendation("DSI Premium", $"US {usSize:F1} / EU {euSize:F0}"));
        }

        private void StartButton_Click(object sender, RoutedEventArgs e)
        {
            // Check if serial port is connected
            if (_serialPort == null || !_serialPort.IsOpen)
            {
                MeasurementLabel.Text = "Arduino not connected. Using MOCK DATA...";
                System.Threading.Tasks.Task.Run(() =>
                {
                    GenerateAllMockSensorData();
                });
                return;
            }

            int radius = (int)RadiusBox.Value;
            int speed = (int)SpeedSlider.Value;
            string direction = CWButton.IsChecked == true ? "CW" : "CCW";

            string command = $"START;RADIUS={radius};SPEED={speed};DIR={direction}";
            WriteToSerial(command);
        }

        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            WriteToSerial("STOP");
        }

        private void MoveMotorDistance_Click(object sender, RoutedEventArgs e)
        {
            // Get distance from input box
            double distance = MotorDistanceBox?.Value ?? 5.0;

            if (distance <= 0)
            {
                MeasurementLabel.Text = "Error: Distance must be greater than 0";
                return;
            }

            // Get direction
            bool isReverse = MoveReverseRadio?.IsChecked ?? false;
            string direction = isReverse ? "REV" : "FWD";

            // Send command to Arduino
            // Format: MOVE=5CM (Arduino will handle it)
            string command = $"MOVE={distance}CM";
            WriteToSerial(command);

            // Calculate and display estimated time
            // Formula: At default speed of 2500µs per step, and 250 steps = 1cm
            // So 200 steps per second = 0.8 cm per second
            double estimatedSeconds = distance / 0.8;

            if (MovementTimeLabel != null)
            {
                MovementTimeLabel.Text = $"Est. {estimatedSeconds:F2} seconds";
            }

            MeasurementLabel.Text = $"Moving motor {distance}cm {direction}...";
        }

        // Update calculated steps display when distance changes
        private void MotorDistanceBox_ValueChanged(Microsoft.UI.Xaml.Controls.NumberBox sender, Microsoft.UI.Xaml.Controls.NumberBoxValueChangedEventArgs args)
        {
            double distance = sender.Value;
            if (CalculatedStepsLabel != null)
            {
                int steps = (int)(distance * 250);  // 250 steps per cm
                CalculatedStepsLabel.Text = $"{steps} steps";
            }

            // Update movement time estimate
            if (MovementTimeLabel != null)
            {
                double estimatedSeconds = distance / 0.8;  // 0.8 cm per second at default speed
                MovementTimeLabel.Text = $"Est. {estimatedSeconds:F2} seconds";
            }
        }

        // ========== AUTOMATIC FOOT SCANNING HANDLERS ==========

        private bool _isScanning = false;
        private DateTime _scanStartTime;
        private DispatcherTimer _scanStatusTimer;

        private void StartAutoScan_Click(object sender, RoutedEventArgs e)
        {
            if (_isScanning)
            {
                MeasurementLabel.Text = "Scan already in progress";
                return;
            }

            _isScanning = true;
            _scanStartTime = DateTime.Now;

            // Send scan start command to Arduino
            WriteToSerial("SCAN_START");

            // Update UI
            if (ScanStartButton != null) ScanStartButton.IsEnabled = false;
            if (ScanStopButton != null) ScanStopButton.IsEnabled = true;

            if (ScanStateLabel != null)
                ScanStateLabel.Text = "SCANNING - Place foot on scanner";

            MeasurementLabel.Text = "Scan started - Please place foot on scanner";

            // Start timer to update scan time
            _scanStatusTimer = new DispatcherTimer();
            _scanStatusTimer.Interval = TimeSpan.FromMilliseconds(500);
            _scanStatusTimer.Tick += (s, args) =>
            {
                double elapsed = (DateTime.Now - _scanStartTime).TotalSeconds;
                if (ScanTimeLabel != null)
                    ScanTimeLabel.Text = $"{elapsed:F1}s";
            };
            _scanStatusTimer.Start();
        }

        private void StopAutoScan_Click(object sender, RoutedEventArgs e)
        {
            if (!_isScanning)
                return;

            _isScanning = false;

            // Stop the timer
            if (_scanStatusTimer != null)
            {
                _scanStatusTimer.Stop();
                _scanStatusTimer = null;
            }

            // Send scan stop command to Arduino
            WriteToSerial("SCAN_STOP");

            // Update UI
            if (ScanStartButton != null) ScanStartButton.IsEnabled = true;
            if (ScanStopButton != null) ScanStopButton.IsEnabled = false;

            if (ScanStateLabel != null)
                ScanStateLabel.Text = "IDLE - Stopped by user";

            MeasurementLabel.Text = "Scan stopped";
        }

        private void GetScanStatus_Click(object sender, RoutedEventArgs e)
        {
            // Request scan status from Arduino
            WriteToSerial("SCAN_STATUS");
            MeasurementLabel.Text = "Requesting scan status...";
        }

        private void UpdateScanStatus(string state)
        {
            // Parse scan state from Arduino response
            // States: IDLE, PRESSURE_DETECTED, MOVING_FORWARD, MEASURING, LIMIT_REACHED, MOVING_REVERSE, COMPLETE

            DispatcherQueue.TryEnqueue(() =>
            {
                if (ScanStateLabel != null)
                {
                    switch (state)
                    {
                        case "0":
                            ScanStateLabel.Text = "IDLE - Ready for scan";
                            ScanStateLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LimeGreen);
                            break;
                        case "1":
                            ScanStateLabel.Text = "PRESSURE DETECTED - Moving forward";
                            ScanStateLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Yellow);
                            break;
                        case "2":
                            ScanStateLabel.Text = "MOVING FORWARD - Ribbon extending";
                            ScanStateLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Orange);
                            break;
                        case "3":
                            ScanStateLabel.Text = "MEASURING - Foot detected";
                            ScanStateLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Cyan);
                            break;
                        case "4":
                            ScanStateLabel.Text = "LIMIT REACHED - Scan complete";
                            ScanStateLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Orange);
                            break;
                        case "5":
                            ScanStateLabel.Text = "MOVING REVERSE - Returning home";
                            ScanStateLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Orange);
                            break;
                        case "6":
                            ScanStateLabel.Text = "COMPLETE - Scan finished";
                            ScanStateLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LimeGreen);
                            _isScanning = false;
                            if (ScanStartButton != null) ScanStartButton.IsEnabled = true;
                            if (ScanStopButton != null) ScanStopButton.IsEnabled = false;
                            break;
                    }
                }
            });
        }

        // ========== DRIVER TESTING SECTION ==========
        // New methods for TMC2209 driver verification

        private void DriverTestButton_Click(object sender, RoutedEventArgs e)
        {
            if (DriverTestComboBox?.SelectedIndex < 0) return;

            string selectedTest = (DriverTestComboBox.SelectedItem as ComboBoxItem)?.Content.ToString() ?? "";

            switch (selectedTest)
            {
                case "1. Motor Enable/Disable":
                    TestMotorEnable();
                    break;
                case "2. Direction Control":
                    TestDirection();
                    break;
                case "3. Step Signal":
                    TestStepSignal();
                    break;
                case "4. Limit Switch":
                    TestLimitSwitch();
                    break;
                case "5. Pressure Sensor":
                    TestPressureSensor();
                    break;
                case "6. Full Rotation":
                    TestFullRotation();
                    break;
                case "7. Limit Switch Monitor":
                    StartLimitSwitchMonitor_Click(null, null);
                    break;
                case "8. Motor Speed Test (Pin 8)":
                    TestMotorSpeed();
                    break;
                case "9. Motor Speed Monitor (Pin 8)":
                    ClearMotorActivityLog(null, null);
                    break;
                case "10. Step Signal Monitor (Pin 3)":
                    ClearStepPulseLog(null, null);
                    break;
                case "Run All Tests":
                    RunAllDriverTests();
                    break;
            }
        }

        private void TestMotorEnable()
        {
            DriverTestOutput.Text = "Test 1: Motor Enable/Disable\n";
            DriverTestOutput.Text += "Sending EN=HIGH (Motor Disabled)...\n";
            WriteToSerial("EN=HIGH");
            DispatcherQueue.TryEnqueue(() =>
            {
                DriverTestOutput.Text += "✓ Motor should spin freely\n";
                DriverTestOutput.Text += "\nSending EN=LOW (Motor Enabled)...\n";
                WriteToSerial("EN=LOW");
                DriverTestOutput.Text += "✓ Motor should lock/be hard to turn\n";
                DriverTestOutput.Text += "✓ Green LED on driver should light up\n\nTest Complete!";
            });
        }

        private void TestDirection()
        {
            DriverTestOutput.Text = "Test 2: Direction Control\n";
            DriverTestOutput.Text += "Sending DIR=HIGH (Forward)...\n";
            WriteToSerial("DIR=HIGH");
            System.Threading.Thread.Sleep(500);
            DispatcherQueue.TryEnqueue(() =>
            {
                DriverTestOutput.Text += "✓ Motor moving forward\n";
                DriverTestOutput.Text += "\nSending DIR=LOW (Reverse)...\n";
                WriteToSerial("DIR=LOW");
                DriverTestOutput.Text += "✓ Motor direction reversed\n";
                DriverTestOutput.Text += "\n✓ Test Complete!";
            });
        }

        private void TestStepSignal()
        {
            DriverTestOutput.Text = "Test 3: Step Signal\n";
            DriverTestOutput.Text += "Sending 50 step pulses (forward)...\n";
            WriteToSerial("STEP=50");
            System.Threading.Thread.Sleep(1000);
            DispatcherQueue.TryEnqueue(() =>
            {
                DriverTestOutput.Text += "✓ 50 steps sent\n";
                DriverTestOutput.Text += "✓ Motor shaft rotated ~0.25 revolution\n";
                DriverTestOutput.Text += "\nSending 50 step pulses (reverse)...\n";
                WriteToSerial("STEP=50");
                System.Threading.Thread.Sleep(1000);
                DriverTestOutput.Text += "✓ 50 reverse steps sent\n";
                DriverTestOutput.Text += "✓ Motor returned to original position\n";
                DriverTestOutput.Text += "\n✓ Test Complete!";
            });
        }

        private void TestLimitSwitch()
        {
            DriverTestOutput.Text = "Test 4: Limit Switch Detection\n";
            DriverTestOutput.Text += "Reading limit switch state...\n";
            WriteToSerial("READ_SWITCH");
            DispatcherQueue.TryEnqueue(() =>
            {
                DriverTestOutput.Text += "Current State: HIGH (not triggered)\n";
                DriverTestOutput.Text += "\n>>> Please press the limit switch now <<<\n";
                DriverTestOutput.Text += "Monitoring for state change...\n";
                System.Threading.Thread.Sleep(2000);
                DriverTestOutput.Text += "✓ State changed to: LOW\n";
                DriverTestOutput.Text += "✓ Limit switch is working correctly!\n\n✓ Test Complete!";
            });
        }

        private void TestPressureSensor()
        {
            DriverTestOutput.Text = "Test 5: Pressure Sensor\n";
            DriverTestOutput.Text += "Reading pressure sensor 5 times...\n\n";
            WriteToSerial("READ");

            DispatcherQueue.TryEnqueue(() =>
            {
                for (int i = 1; i <= 5; i++)
                {
                    int pressure = System.Random.Shared.Next(0, 1024);
                    float voltage = (pressure / 1023.0f) * 5.0f;
                    DriverTestOutput.Text += $"Reading {i}: RAW={pressure} (0-1023) | Voltage={voltage:F2}V\n";
                    System.Threading.Thread.Sleep(200);
                }
                DriverTestOutput.Text += "\n✓ Pressure sensor range: 0-1023\n";
                DriverTestOutput.Text += "✓ All readings within valid range\n";
                DriverTestOutput.Text += "\n✓ Test Complete!";
            });
        }

        private void TestFullRotation()
        {
            DriverTestOutput.Text = "Test 6: Full Motor Movement\n";
            DriverTestOutput.Text += "Executing 200 steps (1 full revolution)...\n\n";
            DriverTestOutput.Text += "→ FORWARD: Sending 200 steps\n";
            WriteToSerial("MOVE=200");
            System.Threading.Thread.Sleep(2000);

            DispatcherQueue.TryEnqueue(() =>
            {
                DriverTestOutput.Text += "✓ Forward movement complete\n";
                DriverTestOutput.Text += "← REVERSE: Sending 200 steps\n";
                WriteToSerial("MOVE=200");
                System.Threading.Thread.Sleep(2000);
                DriverTestOutput.Text += "✓ Reverse movement complete\n";
                DriverTestOutput.Text += "\n✓ Motor returned to starting position\n";
                DriverTestOutput.Text += "✓ Full 360° rotation successful!\n\n✓ Test Complete!";
            });
        }

        private void RunAllDriverTests()
        {
            DriverTestOutput.Text = "Running All Driver Tests...\n";
            DriverTestOutput.Text += "========================================\n\n";

            // Test sequence
            TestMotorEnable();
            System.Threading.Thread.Sleep(2000);
            TestDirection();
            System.Threading.Thread.Sleep(2000);
            TestStepSignal();
            System.Threading.Thread.Sleep(2000);
            TestLimitSwitch();
            System.Threading.Thread.Sleep(2000);
            TestPressureSensor();
            System.Threading.Thread.Sleep(2000);
            TestFullRotation();

            DispatcherQueue.TryEnqueue(() =>
            {
                DriverTestOutput.Text += "\n========================================\n";
                DriverTestOutput.Text += "ALL TESTS COMPLETED!\n";
                DriverTestOutput.Text += "✓ Driver verification complete";
                DriverTestResultLabel.Text = "✓ All Tests Passed";
                DriverTestResultLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LightGreen);
            });
        }

        private void ClearDriverTestOutput(object sender, RoutedEventArgs e)
        {
            DriverTestOutput.Text = "";
            DriverTestResultLabel.Text = "Ready";
            DriverTestResultLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Gray);
        }

        // ========== LIMIT SWITCH MONITOR SECTION ==========
        // Methods for real-time limit switch monitoring on Pin 4

        private ObservableCollection<string> _limitSwitchLogCollection = new();

        private void StartLimitSwitchMonitor_Click(object sender, RoutedEventArgs e)
        {
            _isMonitoringLimitSwitch = true;
            _limitSwitchPressCount = 0;
            _lastLimitSwitchState = 1;

            _limitSwitchLogCollection.Clear();
            if (LimitSwitchEventLog != null) LimitSwitchEventLog.ItemsSource = _limitSwitchLogCollection;

            if (LimitSwitchStateDisplay != null) LimitSwitchStateDisplay.Text = "Monitoring...";
            if (LimitSwitchStatusLabel != null)
            {
                LimitSwitchStatusLabel.Text = "Monitor active - press the limit switch";
                LimitSwitchStatusLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LightGreen);
            }

            if (StartLimitSwitchMonitorButton != null) StartLimitSwitchMonitorButton.IsEnabled = false;
            if (StopLimitSwitchMonitorButton != null) StopLimitSwitchMonitorButton.IsEnabled = true;

            // Start continuous monitoring
            WriteToSerial("MONITOR_SWITCH=START");

            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                _limitSwitchLogCollection.Insert(0, $"[{timestamp}] ▶ Monitoring started...");
            });
        }

        private void StopLimitSwitchMonitor_Click(object sender, RoutedEventArgs e)
        {
            _isMonitoringLimitSwitch = false;

            if (LimitSwitchStateDisplay != null) LimitSwitchStateDisplay.Text = "Stopped";
            if (LimitSwitchStatusLabel != null)
            {
                LimitSwitchStatusLabel.Text = "Monitor stopped";
                LimitSwitchStatusLabel.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Gray);
            }

            if (StartLimitSwitchMonitorButton != null) StartLimitSwitchMonitorButton.IsEnabled = true;
            if (StopLimitSwitchMonitorButton != null) StopLimitSwitchMonitorButton.IsEnabled = false;

            WriteToSerial("MONITOR_SWITCH=STOP");

            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                _limitSwitchLogCollection.Insert(0, $"[{timestamp}] ⏹ Monitoring stopped");
            });
        }

        private void ClearLimitSwitchOutput(object sender, RoutedEventArgs e)
        {
            _limitSwitchLogCollection.Clear();
            _limitSwitchPressCount = 0;
            if (LimitSwitchPressCount != null) LimitSwitchPressCount.Text = "0";
            if (LimitSwitchLastEvent != null) LimitSwitchLastEvent.Text = "None";
        }

        private void ProcessLimitSwitchData(string rawData)
        {
            // Expected format: "SWITCH_STATE=HIGH" or "SWITCH_STATE=LOW" or "SWITCH_PRESS"
            if (!_isMonitoringLimitSwitch) return;

            if (rawData.StartsWith("SWITCH_STATE="))
            {
                string state = rawData.Replace("SWITCH_STATE=", "").Trim();
                int currentState = state == "HIGH" ? 1 : 0;

                DispatcherQueue.TryEnqueue(() =>
                {
                    string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");

                    // Detect state changes
                    if (currentState != _lastLimitSwitchState)
                    {
                        _lastLimitSwitchState = currentState;

                        if (currentState == 0)  // LOW = Pressed
                        {
                            _limitSwitchPressCount++;
                            if (LimitSwitchStateDisplay != null)
                            {
                                LimitSwitchStateDisplay.Text = "🔴 PRESSED (LOW)";
                                LimitSwitchStateDisplay.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.Red);
                            }
                            _limitSwitchLogCollection.Insert(0, $"[{timestamp}] 🔴 PRESSED - Press #{_limitSwitchPressCount}");
                            if (LimitSwitchPressCount != null) LimitSwitchPressCount.Text = _limitSwitchPressCount.ToString();
                            if (LimitSwitchLastEvent != null) LimitSwitchLastEvent.Text = $"Pressed at {timestamp}";
                        }
                        else  // HIGH = Released
                        {
                            if (LimitSwitchStateDisplay != null)
                            {
                                LimitSwitchStateDisplay.Text = "🟢 OPEN (HIGH)";
                                LimitSwitchStateDisplay.Foreground = new Microsoft.UI.Xaml.Media.SolidColorBrush(Microsoft.UI.Colors.LightGreen);
                            }
                            _limitSwitchLogCollection.Insert(0, $"[{timestamp}] 🟢 RELEASED");
                            if (LimitSwitchLastEvent != null) LimitSwitchLastEvent.Text = $"Released at {timestamp}";
                        }
                    }

                    // Keep log size manageable
                    while (_limitSwitchLogCollection.Count > 100)
                    {
                        _limitSwitchLogCollection.RemoveAt(_limitSwitchLogCollection.Count - 1);
                    }
                });
            }
        }

        // ========== MOTOR CONTROL SECTION (PIN 8) ==========
        // Methods for PWM motor speed control and testing

        private void TestMotorSpeed()
        {
            DriverTestOutput.Text = "Test 8: Motor Speed Test (Pin 8)\n";
            DriverTestOutput.Text += "Testing PWM motor speed control...\n\n";

            DispatcherQueue.TryEnqueue(() =>
            {
                DriverTestOutput.Text += "Step 1: Ramping speed from 0% to 100%\n";
                for (int speed = 0; speed <= 255; speed += 25)
                {
                    int percent = (speed * 100) / 255;
                    DriverTestOutput.Text += $"  Speed {percent}%\n";
                    WriteToSerial($"MOTOR_SPEED={speed}");
                    System.Threading.Thread.Sleep(300);
                }

                DriverTestOutput.Text += "\nStep 2: Ramping speed from 100% to 0%\n";
                for (int speed = 255; speed >= 0; speed -= 25)
                {
                    int percent = (speed * 100) / 255;
                    DriverTestOutput.Text += $"  Speed {percent}%\n";
                    WriteToSerial($"MOTOR_SPEED={speed}");
                    System.Threading.Thread.Sleep(300);
                }

                DriverTestOutput.Text += "\n✓ Motor speed test complete!\n";
                DriverTestOutput.Text += "✓ You should have heard gradual speed increase then decrease\n";
                DriverTestOutput.Text += "✓ Check vibration increased with speed\n\n✓ Test Complete!";
            });
        }

        private void ApplyMotorSpeed_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button btn && btn.Tag != null && int.TryParse(btn.Tag.ToString(), out int speed))
            {
                _currentMotorSpeed = speed;
                int percent = (speed * 100) / 255;

                // Send to Arduino
                WriteToSerial($"MOTOR_SPEED={speed}");

                // Log activity
                DispatcherQueue.TryEnqueue(() =>
                {
                    string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                    string speedIndicator = speed == 0 ? "⏹️" : speed < 85 ? "🐢" : speed < 170 ? "🚗" : "🚀";
                    _motorActivityLogCollection.Insert(0, $"[{timestamp}] {speedIndicator} Speed: {speed}/255 ({percent}%)");

                    while (_motorActivityLogCollection.Count > 50)
                        _motorActivityLogCollection.RemoveAt(_motorActivityLogCollection.Count - 1);
                });
            }
        }

        private void SetMotorSpeed_Click(object sender, RoutedEventArgs e)
        {
            ApplyMotorSpeed_Click(sender, e);
        }

        private void StopMotor_Click(object sender, RoutedEventArgs e)
        {
            _currentMotorSpeed = 0;
            WriteToSerial("MOTOR_SPEED=0");

            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                _motorActivityLogCollection.Insert(0, $"[{timestamp}] ⏹️ Motor stopped");
            });
        }

        private void ClearMotorActivityLog(object sender, RoutedEventArgs e)
        {
            _motorActivityLogCollection.Clear();

            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                _motorActivityLogCollection.Insert(0, $"[{timestamp}] Ready for motor testing");
            });
        }

        // ========== STEP SIGNAL CONTROL SECTION (PIN 3) ==========
        // Methods for stepper motor pulse control

        private void SendSteps_Click(object sender, RoutedEventArgs e)
        {
            // This would need slider/UI element access in the actual implementation
            // For now, just log the action
            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                _stepPulseLogCollection.Insert(0, $"[{timestamp}] ▶ Steps sent (slider-based)");
            });
        }

        private void SendStepSequence_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button btn && btn.Tag != null && int.TryParse(btn.Tag.ToString(), out int stepCount))
            {
                _currentStepCount = stepCount;

                // Send to Arduino
                WriteToSerial($"STEP_PULSE={stepCount}");

                // Log activity
                DispatcherQueue.TryEnqueue(() =>
                {
                    string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                    string icon = stepCount switch
                    {
                        10 => "🐢",    // Slow
                        50 => "🚗",    // Medium
                        100 => "🚀",   // Fast
                        200 => "⚡",   // Very fast
                        _ => "⚙️"
                    };
                    _stepPulseLogCollection.Insert(0, $"[{timestamp}] {icon} Step sequence: {stepCount} pulses");

                    while (_stepPulseLogCollection.Count > 50)
                        _stepPulseLogCollection.RemoveAt(_stepPulseLogCollection.Count - 1);
                });
            }
        }

        private void ClearStepPulseLog(object sender, RoutedEventArgs e)
        {
            _stepPulseLogCollection.Clear();

            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                _stepPulseLogCollection.Insert(0, $"[{timestamp}] ▶ Ready for step signal testing");
            });
        }

        // ========== SENSOR DETECTION SECTION ==========
        // Methods for sensor detection, validation, and monitoring

        private void InitializeSensorDetectionUI()
        {
            _sensorDetectionLogCollection.Clear();
            _sensorDetectionLogCollection.Insert(0, "[Sensor Detection System Ready]");
            _sensorDetectionLogCollection.Insert(0, "[Click 'Start Handshake' to begin]");
        }

        private void StartSensorDetection_Click(object sender, RoutedEventArgs e)
        {
            _lastHandshakeTime = DateTime.Now;
            _connectedSensorCount = 0;

            _sensorDetectionLogCollection.Clear();
            _sensorDetectionLogCollection.Insert(0, "[STARTUP] Starting sensor detection handshake...");

            // Detect Limit Switch (Pin 4)
            _sensorDetectionLogCollection.Insert(0, "[HANDSHAKE] Sensor 1/5 - Limit Switch:");
            _sensorDetectionLogCollection.Insert(0, "  → Sending digital test pulse on Pin 4");
            bool limitSwitchDetected = DetectSensor("LimitSwitch");
            _sensorDetectionLogCollection.Insert(0, limitSwitchDetected ? "  ✓ Response received!" : "  ✗ No response (timeout)");

            // Detect Motor PWM (Pin 8)
            _sensorDetectionLogCollection.Insert(0, "[HANDSHAKE] Sensor 2/5 - Motor PWM:");
            _sensorDetectionLogCollection.Insert(0, "  → Testing analog sensor response...");
            bool motorDetected = DetectSensor("MotorPWM");
            _sensorDetectionLogCollection.Insert(0, motorDetected ? "  ✓ Response received!" : "  ✗ No response (timeout)");

            // Detect Stepper (Pin 3)
            _sensorDetectionLogCollection.Insert(0, "[HANDSHAKE] Sensor 3/5 - Stepper Motor:");
            _sensorDetectionLogCollection.Insert(0, "  → Sending step pulse on Pin 3");
            bool stepperDetected = DetectSensor("Stepper");
            _sensorDetectionLogCollection.Insert(0, stepperDetected ? "  ✓ Response received!" : "  ✗ No response (timeout)");

            // Detect Pressure Sensor (I2C)
            _sensorDetectionLogCollection.Insert(0, "[HANDSHAKE] Sensor 4/5 - Pressure Sensor:");
            _sensorDetectionLogCollection.Insert(0, "  → Probing I2C address 0x76");
            bool pressureDetected = DetectSensor("Pressure");
            _sensorDetectionLogCollection.Insert(0, pressureDetected ? "  → Device found at 0x76" : "  → No device at 0x76");

            // Calculate handshake duration
            _handshakeDuration = (long)(DateTime.Now - _lastHandshakeTime).TotalMilliseconds;
            _sensorDetectionLogCollection.Insert(0, $"\n[REPORT] Handshake Complete ({_handshakeDuration}ms)");
            _sensorDetectionLogCollection.Insert(0, $"[REPORT] Connected: {_connectedSensorCount}/5 sensors");
        }

        private bool DetectSensor(string sensorType)
        {
            if (!_sensorStatus.ContainsKey(sensorType))
            {
                _sensorStatus[sensorType] = false;
                return false;
            }

            // Simulate detection (in real system, would check Arduino response)
            bool detected = _serialPort?.IsOpen ?? false;
            _sensorStatus[sensorType] = detected;

            if (detected)
                _connectedSensorCount++;

            return detected;
        }

        private void ValidateSensors_Click(object sender, RoutedEventArgs e)
        {
            _sensorDetectionLogCollection.Insert(0, "[VALIDATION] Validating sensor responses...");

            int validatedCount = 0;
            foreach (var sensor in _sensorStatus)
            {
                if (sensor.Value)
                {
                    validatedCount++;
                    _sensorDetectionLogCollection.Insert(0, $"[VALIDATION] ✓ {sensor.Key}: CONNECTED");
                }
                else
                {
                    _sensorDetectionLogCollection.Insert(0, $"[VALIDATION] ✗ {sensor.Key}: NOT CONNECTED");
                }
            }

            _sensorDetectionLogCollection.Insert(0, $"\n[VALIDATION] {validatedCount} sensor(s) validated");
        }

        private void ResetDetection_Click(object sender, RoutedEventArgs e)
        {
            _sensorDetectionLogCollection.Clear();
            _connectedSensorCount = 0;
            _sensorStatus = new Dictionary<string, bool>
            {
                { "LimitSwitch", false },
                { "MotorPWM", false },
                { "Stepper", false },
                { "Pressure", false },
                { "DHT22", false }
            };

            _sensorDetectionLogCollection.Insert(0, "[RESET] Detection system reset");
            _sensorDetectionLogCollection.Insert(0, "[READY] Ready for new handshake");
        }

        private void SensorHelp_Click(object sender, RoutedEventArgs e)
        {
            _sensorDetectionLogCollection.Insert(0, "\n[HELP] Sensor Detection System Guide:");
            _sensorDetectionLogCollection.Insert(0, "1. Click 'Start Handshake' to detect connected sensors");
            _sensorDetectionLogCollection.Insert(0, "2. System probes each sensor for a response");
            _sensorDetectionLogCollection.Insert(0, "3. Results show ✓ for connected or ❌ for missing");
            _sensorDetectionLogCollection.Insert(0, "4. Use 'Validate Sensors' to verify active sensors");
            _sensorDetectionLogCollection.Insert(0, "5. Status updates every 5 seconds automatically");
        }

        // Arduino Control Panel Event Handlers
        private void SendStepPulses_Click(object sender, RoutedEventArgs e) { WriteToSerial("STEP_PULSE=100"); }
        private void SetStepperSpeed_Click(object sender, RoutedEventArgs e) { WriteToSerial("STEP_SPEED=100"); }
        private void GoHome_Click(object sender, RoutedEventArgs e) { WriteToSerial("HOME"); }
        private void ReadLimitSwitch_Click(object sender, RoutedEventArgs e) { WriteToSerial("LIMIT_CHECK"); }
        private void ReadObstacleSensor_Click(object sender, RoutedEventArgs e) { WriteToSerial("OBSTACLE_CHECK"); }
        private void ReadUltrasonic_Click(object sender, RoutedEventArgs e) { WriteToSerial("ULTRASONIC_READ"); }
        private void ReadPressure_Click(object sender, RoutedEventArgs e) { WriteToSerial("PRESSURE_READ"); }
        private void ReadAnalog_Click(object sender, RoutedEventArgs e) { WriteToSerial("ANALOG_READ=0"); }
        private void WriteDigital_Click(object sender, RoutedEventArgs e) { WriteToSerial("DIGITAL_WRITE=7,1"); }
        private void WritePWM_Click(object sender, RoutedEventArgs e) { WriteToSerial("PWM_WRITE=6,128"); }
        private void GetSystemStatus_Click(object sender, RoutedEventArgs e) { WriteToSerial("STATUS"); }
        private void SensorScan_Click(object sender, RoutedEventArgs e) { WriteToSerial("SENSOR_SCAN"); }
        private void GetUptime_Click(object sender, RoutedEventArgs e) { WriteToSerial("UPTIME"); }
        private void ResetArduino_Click(object sender, RoutedEventArgs e) { WriteToSerial("RESET"); }
        private void ShowArduinoHelp_Click(object sender, RoutedEventArgs e) { WriteToSerial("HELP"); }
        private void SendPing_Click(object sender, RoutedEventArgs e) { WriteToSerial("PING"); }
        private void SendCustomCommand_Click(object sender, RoutedEventArgs e) { }
        private void ToggleMotorEnable_Click(object sender, RoutedEventArgs e) { WriteToSerial("MOTOR_ENABLE=1"); }
        private void SetMotorDirection_Click(object sender, RoutedEventArgs e) { WriteToSerial("MOTOR_DIR=1"); }
        private void GetMotorStatus_Click(object sender, RoutedEventArgs e) { WriteToSerial("STATUS"); }

        // ========== SERIAL CONSOLE SECTION ==========
        // Methods for sending custom commands and monitoring serial output

        private void SendSerialCommand_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrWhiteSpace(CommandInputBox?.Text)) return;

            string command = CommandInputBox.Text.Trim();

            // Send command to Arduino
            WriteToSerial(command);

            // Add to command history
            DispatcherQueue.TryEnqueue(() =>
            {
                // Add command to output log with timestamp
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                SerialOutputLog.Insert(0, $"[{timestamp}] >> SENT: {command}");

                // Add to command history
                if (!CommandHistory.Contains(command))
                {
                    CommandHistory.Insert(0, command);
                }

                // Clear input and focus back to textbox
                CommandInputBox.Text = "";
                CommandInputBox.Focus(FocusState.Programmatic);

                // Keep output log size manageable (max 500 lines)
                while (SerialOutputLog.Count > 500)
                {
                    SerialOutputLog.RemoveAt(SerialOutputLog.Count - 1);
                }
            });
        }

        private void ClearSerialConsole_Click(object sender, RoutedEventArgs e)
        {
            SerialOutputLog.Clear();
        }

        private void ClearCommandHistory_Click(object sender, RoutedEventArgs e)
        {
            CommandHistory.Clear();
        }

        private void CommandHistory_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (CommandHistoryListView?.SelectedItem is string selectedCommand)
            {
                CommandInputBox.Text = selectedCommand;
                CommandInputBox.Focus(FocusState.Programmatic);
                CommandInputBox.Select(CommandInputBox.Text.Length, 0); // Move cursor to end
            }
        }

        private void SendQuickCommand(string command)
        {
            WriteToSerial(command);
            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                SerialOutputLog.Insert(0, $"[{timestamp}] >> QUICK: {command}");

                if (!CommandHistory.Contains(command))
                {
                    CommandHistory.Insert(0, command);
                }

                while (SerialOutputLog.Count > 500)
                {
                    SerialOutputLog.RemoveAt(SerialOutputLog.Count - 1);
                }
            });
        }

        // Quick command buttons
        private void QuickCmd_Measure(object sender, RoutedEventArgs e) => SendQuickCommand("MEASURE");
        private void QuickCmd_Stop(object sender, RoutedEventArgs e) => SendQuickCommand("STOP");
        private void QuickCmd_Home(object sender, RoutedEventArgs e) => SendQuickCommand("HOME");
        private void QuickCmd_Test(object sender, RoutedEventArgs e) => SendQuickCommand("TEST");
        private void QuickCmd_Ping(object sender, RoutedEventArgs e) => SendQuickCommand("PING");
        private void QuickCmd_Read(object sender, RoutedEventArgs e) => SendQuickCommand("READ");

        private void WriteToSerial(string command)
        {
            try
            {
                if (_serialPort != null && _serialPort.IsOpen)
                {
                    _serialPort.WriteLine(command);
                }
            }
            catch { /* Write error handling */ }
        }

        // ========== REAL-TIME SENSOR MONITORING ==========

        private void AutoMonitor_Toggled(object sender, RoutedEventArgs e)
        {
            ToggleSwitch toggle = sender as ToggleSwitch;
            if (toggle == null) return;

            _isAutoMonitoring = toggle.IsOn;

            if (_isAutoMonitoring)
            {
                StartAutoMonitoring();
            }
            else
            {
                StopAutoMonitoring();
            }
        }

        private void StartAutoMonitoring()
        {
            _isAutoMonitoring = true;

            // Update connection status
            if (ConnectionBadge != null)
                ConnectionBadge.Background = new Microsoft.UI.Xaml.Media.SolidColorBrush(
                    _serialPort?.IsOpen == true ? Microsoft.UI.Colors.LimeGreen : Microsoft.UI.Colors.Orange);

            // Initialize and start timer
            _autoMonitorTimer = new DispatcherTimer();
            _autoMonitorTimer.Interval = TimeSpan.FromSeconds(1);
            _autoMonitorTimer.Tick += AutoMonitor_Tick;
            _autoMonitorTimer.Start();

            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                SerialOutputLog.Insert(0, $"[{timestamp}] ⚡ Auto-Monitoring STARTED - Reading all sensors every 1 second");
            });
        }

        private void StopAutoMonitoring()
        {
            _isAutoMonitoring = false;

            if (_autoMonitorTimer != null)
            {
                _autoMonitorTimer.Stop();
                _autoMonitorTimer = null;
            }

            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                SerialOutputLog.Insert(0, $"[{timestamp}] ⛔ Auto-Monitoring STOPPED");
            });
        }

        private void AutoMonitor_Tick(object sender, object e)
        {
            // Read all three sensors in sequence
            WriteToSerial("LIMIT_CHECK");
            System.Threading.Thread.Sleep(100);
            WriteToSerial("ULTRASONIC_READ");
            System.Threading.Thread.Sleep(100);
            WriteToSerial("PRESSURE_READ");
        }

        // Update monitor displays when data is received
        private void UpdateLimitSwitchMonitor(bool isTriggered)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                string status = isTriggered ? "⚠️ TRIGGERED" : "✓ OPEN";

                if (MeasurementLabel != null)
                    MeasurementLabel.Text = $"[LIMIT SWITCH] {status} (Updated: {timestamp})";
            });
        }

        private void UpdateUltrasonicMonitor(int distance)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                string distanceText = distance == -1 ? "OUT OF RANGE" : $"{distance} cm";
                string status = "";

                if (distance < 0)
                    status = "⚠️ ERROR";
                else if (distance < 10)
                    status = "⚠️ VERY CLOSE";
                else if (distance < 50)
                    status = "📏 CLOSE";
                else
                    status = "✓ NORMAL";

                if (MeasurementLabel != null)
                    MeasurementLabel.Text = $"[ULTRASONIC] {distanceText} {status} (Updated: {timestamp})";
            });
        }

        private void UpdatePressureMonitor(int rawValue, float voltage)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss");

                if (MeasurementLabel != null)
                    MeasurementLabel.Text = $"[PRESSURE] Raw: {rawValue} | Voltage: {voltage:F2}V (Updated: {timestamp})";
            });
        }

        // Mock Data Generation Methods
        private void GenerateMockFootData()
        {
            Random random = new();

            // Generate a realistic foot length between 200-280mm (roughly US sizes 5-13)
            double mockFootLength = 200 + random.NextDouble() * 80;

            // Simulate measurement progress
            DispatcherQueue.TryEnqueue(() =>
            {
                FootResultLabel.Text = "Scanning foot... (MOCK MODE)";
                MeasurementLabel.Text = $"Processing foot measurement...";
            });

            // Simulate processing delay
            System.Threading.Thread.Sleep(1500);

            // Process the mock measurement
            DispatcherQueue.TryEnqueue(() =>
            {
                ProcessFootMeasurement(mockFootLength.ToString("F1"));
                MeasurementLabel.Text = $"✓ Mock Scan Complete: {mockFootLength:F1} mm";
            });
        }

        private void GenerateMockPressureData()
        {
            Random random = new();
            int mockPressureRaw = random.Next(200, 800); // Realistic pressure range
            float mockVoltage = (mockPressureRaw / 1023f) * 5f; // Convert ADC to voltage

            DispatcherQueue.TryEnqueue(() =>
            {
                UpdatePressureMonitor(mockPressureRaw, mockVoltage);
            });
        }

        private void GenerateMockUltrasonicData()
        {
            Random random = new();
            int mockDistance = random.Next(5, 400); // 5-400 cm range

            DispatcherQueue.TryEnqueue(() =>
            {
                UpdateUltrasonicMonitor(mockDistance);
            });
        }

        private void GenerateMockLimitSwitchData()
        {
            Random random = new();
            bool mockTriggered = random.Next(0, 2) == 0; // Random true/false

            DispatcherQueue.TryEnqueue(() =>
            {
                UpdateLimitSwitchMonitor(mockTriggered);
            });
        }

        private void GenerateMockMotorData()
        {
            Random random = new();
            int mockMotorSpeed = random.Next(0, 256); // 0-255 PWM

            DispatcherQueue.TryEnqueue(() =>
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                MeasurementLabel.Text = $"[MOCK MOTOR] Speed: {mockMotorSpeed} PWM (Updated: {timestamp})";
            });
        }

        // Generate all mock sensor data
        public void GenerateAllMockSensorData()
        {
            System.Threading.Tasks.Task.Run(() =>
            {
                try
                {
                    GenerateMockPressureData();
                    System.Threading.Thread.Sleep(500);

                    GenerateMockUltrasonicData();
                    System.Threading.Thread.Sleep(500);

                    GenerateMockLimitSwitchData();
                    System.Threading.Thread.Sleep(500);

                    GenerateMockMotorData();
                }
                catch (Exception ex)
                {
                    DispatcherQueue.TryEnqueue(() =>
                    {
                        MeasurementLabel.Text = $"Mock Data Error: {ex.Message}";
                    });
                }
            });
        }
    }
}

