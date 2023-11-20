using System.IO.Ports;

using PluginManager.Abstractions;

namespace OpOverkillShared
{
    public sealed class WindowsComPort
    {
        private readonly SerialPort _serialPort;

        public WindowsComPort(ISettingsProvider settingsProvider)
        {
            Settings settings = settingsProvider.GetSettings<Settings>("Settings");


            Name = settings.ComPort;

            if (!Enum.TryParse<Parity>(settings.Parity, out Parity parity))
                parity = Parity.None;

            if (!Enum.TryParse<StopBits>(settings.StopBits, out StopBits stopBits))
                stopBits = StopBits.One;

            _serialPort = new SerialPort(settings.ComPort, settings.BaudRate,
                parity, settings.DataBits, stopBits);

#if DEBUG
            _serialPort.ReadTimeout = settings.ReadTimeout * 5;
            _serialPort.WriteTimeout = settings.WriteTimeout * 5;
#else
            _serialPort.ReadTimeout = settings.ReadTimeout;
            _serialPort.WriteTimeout = settings.WriteTimeout;
#endif

            _serialPort.DataReceived += SerialPort_DataReceived;
            _serialPort.ErrorReceived += SerialPort_ErrorReceived;
            _serialPort.PinChanged += SerialPort_PinChanged;
        }

        public string Name { get; }

        public void Close()
        {
            if (IsOpen())
            {
                _serialPort.Close();
            }
        }

        public bool IsOpen()
        {
            return _serialPort.IsOpen;
        }

        public void Open()
        {
            if (!IsOpen())
            {
                _serialPort.Open();

                //discard initial bytes
                if (_serialPort.BytesToRead > 0)
                {
                    _ = _serialPort.ReadExisting();
                }
            }
        }

        public bool CanReadLine()
        {
            return IsOpen() && _serialPort.BytesToRead > 0;
        }

        public string ReadLine()
        {
            if (IsOpen() && _serialPort.BytesToRead > 0)
            {
                return _serialPort.ReadLine();
            }

            return String.Empty;
        }

        public void WriteLine(string line)
        {
            if (IsOpen() && !String.IsNullOrEmpty(line))
            {
                _serialPort.WriteLine(line);
            }
        }

        public void Write(byte[] buffer, int offset, int count)
        {
            if (IsOpen() && buffer.Length > 0)
            {
                _serialPort.Write(buffer, offset, count);
            }
        }

        public event SerialErrorReceivedEventHandler ErrorReceived;

        public event SerialPinChangedEventHandler PinChanged;

        public event EventHandler DataReceived;

        private void SerialPort_PinChanged(object sender, SerialPinChangedEventArgs e) => PinChanged?.Invoke(sender, e);

        private void SerialPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e) => ErrorReceived?.Invoke(sender, e);

        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e) => DataReceived?.Invoke(sender, e);
    }
}
