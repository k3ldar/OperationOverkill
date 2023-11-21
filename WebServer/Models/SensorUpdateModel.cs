using System;

using OpOverkillShared;

namespace OpOverkillWebServer.Models
{
    public sealed class SensorUpdateModel
    {
        public SensorUpdateModel(IArduinoProcessor arduinoProcessor)
        {
            if (arduinoProcessor == null)
                throw new ArgumentNullException(nameof(arduinoProcessor));

            Pump1Active = arduinoProcessor.Pump1Active;
            Pump2Active = arduinoProcessor.Pump2Active;
            SensorValue = arduinoProcessor.SensorValue;
            RelayCount = arduinoProcessor.RelayCount;

            Time = DateTime.Now.ToString("HH:mm:ss");
        }

        public bool Pump1Active { get; }

        public bool Pump2Active { get; }

        public int SensorValue { get; }

        public int RelayCount { get; }

        public string Time { get; }
    }
}
