using PluginManager.Abstractions;

namespace OpOverkillShared
{
    public sealed class ArduinoProcessor : IArduinoProcessor
    {
        private readonly WindowsComPort _comport;
        private bool _pump1Active;
        private bool _pump2Active;
        private int _sensorValue = -1;
        private int _relayCount;

        public ArduinoProcessor(WindowsComPort comPort)
        {
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

                _sensorValue = value;
                SensorValueChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        public int RelayCount
        {
            get => _relayCount;

            private set
            {
                if (_relayCount == value)
                    return;

                _relayCount = value;
                RelayValueChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        public event EventHandler Pump1ActiveChanged;
        public event EventHandler Pump2ActiveChanged;
        public event EventHandler SensorValueChanged;
        public event EventHandler RelayValueChanged;

        private void Comport_DataReceived(object sender, EventArgs e)
        {
            while (_comport.CanReadLine())
            {
                string data = _comport.ReadLine();
                string[] parts = data.Split('/', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);

                if (parts.Length != 5)
                    return;

                string[] header = parts[0].Split(':');

                if (header.Length != 2)
                    return;

                if (header[0] != "RF")
                    return;

                if (header[1] == "WS")
                {
                    ProcessWaterSensorMonitor(parts);
                }
            }
        }

        private void ProcessWaterSensorMonitor(string[] parts)
        {
            SensorValue = Convert.ToInt32(parts[1]);
            RelayCount = Convert.ToInt32(parts[2]);
            Pump1Active = Convert.ToInt32(parts[3]) != 0;
            Pump2Active = Convert.ToInt32(parts[4]) != 0;
#if DEBUG
            Console.WriteLine($"{DateTime.Now.ToString("HH:mm:ss")} Sensor value: {SensorValue}; Relay Count: {RelayCount}; Pump 1 Active: {Pump1Active}; Pump 2 Active: {Pump2Active}");
#endif
        }
    }
}
