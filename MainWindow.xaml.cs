using Microsoft.UI.Xaml;
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

    public sealed partial class MainWindow : Window
    {
        private SerialPort? _serialPort;
        public ObservableCollection<double> DataValues { get; set; } = new();
        public ObservableCollection<ShoeRecommendation> Recommendations { get; set; } = new();

        public IEnumerable<ISeries> Series { get; set; }
        public IEnumerable<ICartesianAxis> XAxes { get; set; }
        public IEnumerable<ICartesianAxis> YAxes { get; set; }

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
            RefreshPorts();
        }

        private void MeasureButton_Click(object sender, RoutedEventArgs e)
        {
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
                if (rawData.StartsWith("FOOT="))
                {
                    ProcessFootMeasurement(rawData.Replace("FOOT=", ""));
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
                FootResultLabel.Text = $"Measured Length: {length:F1} mm";
                CalculateRecommendations(length);
            }
        }

        private void CalculateRecommendations(double mmStep)
        {
            Recommendations.Clear();

            // Basic US Men's scale: Size = (LengthInInches * 3) - 22
            double usSize = (mmStep / 25.4) * 3 - 22;
            // Basic EU scale: (mm + 10) / 6.67
            double euSize = (mmStep + 10) / 6.67;

            Recommendations.Add(new ShoeRecommendation("Nike", $"US {(usSize + 0.5):F1} / EU {euSize:F0}"));
            Recommendations.Add(new ShoeRecommendation("Adidas", $"US {usSize:F1} / EU {(euSize + 1):F0}"));
            Recommendations.Add(new ShoeRecommendation("Puma", $"US {(usSize - 0.5):F1} / EU {euSize:F0}"));
            Recommendations.Add(new ShoeRecommendation("Brooks", $"US {usSize:F1} / EU {euSize:F0} (Running Fit)"));
        }

        private void StartButton_Click(object sender, RoutedEventArgs e)
        {
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
    }
}
