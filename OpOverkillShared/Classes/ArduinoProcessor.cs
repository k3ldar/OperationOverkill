using System.Collections.Concurrent;
using System.Net.Http.Headers;

using OpOverkillShared.Abstractions;
using OpOverkillShared.Classes;

using Shared.Classes;

namespace OpOverkillShared
{
    public sealed class ArduinoProcessor : IArduinoProcessor
    {
        private readonly WindowsComPort _comport;
        private bool _pump1Active;
        private bool _pump2Active;
        private int _sensorValue = -1;
        private int _sensorAverage = 0;
        private double _temperature = -99.9;

        public ArduinoProcessor(WindowsComPort comPort, ApiWrapper apiWrapper)
        {
            WeatherUpdateThread weatherUpdateThread = new(apiWrapper, this);
            ThreadManager.ThreadStart(weatherUpdateThread, "Weather Update Thread", ThreadPriority.BelowNormal);

            _comport = comPort ?? throw new ArgumentNullException(nameof(comPort));
            _comport.DataReceived += Comport_DataReceived;
        }

        public void Connect()
        {
            if (!_comport.IsOpen())
                _comport.Open();
        }

        public bool Pump1Active
        {
            get => _pump1Active;

            private set
            {
                if (_pump1Active == value)
                    return;

                _pump1Active = value;
                Pump1ActiveChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        public bool Pump2Active
        {
            get => _pump2Active;

            private set
            {
                if (_pump2Active == value)
                    return;

                _pump2Active = value;
                Pump2ActiveChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        public int SensorValue
        {
            get => _sensorValue;

            private set
            {
                if (_sensorValue == value)
                    return;

                if (_sensorValue != value)
                {
                    _sensorValue = value;
                    SensorValueChanged?.Invoke(this, EventArgs.Empty);
                }
            }
        }

        public int SensorAverage
        {
            get
            {
                return _sensorAverage;
            }

            set
            {
                if (_sensorAverage == value)
                    return;

                _sensorAverage = value;
                SensorAverageValueChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        public double TemperatureForcast
        {
            get
            {
                return _temperature;
            }

            set
            {
                double newValue = value;

                var weatherData = WeatherUpdateThread.LatestWeatherData.CurrentData();

                if (_temperature < -50 && weatherData != null && WeatherUpdateThread.LatestWeatherData.LastUpdated > DateTime.UtcNow.AddMinutes(-6))
                {
                    Double.TryParse(weatherData.Temperature_2m.Substring(0, weatherData.Temperature_2m.Length -2), out newValue);
                    _comport.WriteLine($"T1:t={newValue}");
                }

                if (newValue == _temperature)
                    return;

                _temperature = newValue;
                SensorTemperatureChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        public event EventHandler Pump1ActiveChanged;
        public event EventHandler Pump2ActiveChanged;
        public event EventHandler SensorValueChanged;
        public event EventHandler SensorAverageValueChanged;
        public event EventHandler SensorTemperatureChanged;

        private void Comport_DataReceived(object sender, EventArgs e)
        {
            while (_comport.CanReadLine())
            {
                string data = _comport.ReadLine();
                string[] parts = data.Split('/', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);

                if (parts.Length != 7)
                    return;

                string[] header = parts[0].Split(':');

                if (header.Length != 2)
                    return;

                if (header[0] != "RF")
                    return;

                if (parts[1] == "WS")
                {
                    ProcessWaterSensorMonitor(parts);
                }
            }
        }

        private void ProcessWaterSensorMonitor(string[] parts)
        {
            SensorValue = Convert.ToInt32(parts[2]);
            SensorAverage = Convert.ToInt32(parts[3]);
            TemperatureForcast = Convert.ToDouble(parts[4]);
            Pump1Active = Convert.ToInt32(parts[5]) != 0;
            Pump2Active = Convert.ToInt32(parts[6]) != 0;
#if DEBUG
            Console.WriteLine($"{DateTime.Now.ToString("HH:mm:ss")} Sensor value: {SensorValue}; Sensor Average: {SensorAverage}; Pump 1 Active: {Pump1Active}; Pump 2 Active: {Pump2Active}");
#endif
        }
    }
}
