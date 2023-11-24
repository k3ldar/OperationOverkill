using System;

using OpOverkillShared;
using OpOverkillShared.Classes;

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
            Average30Seconds = Double.Round(arduinoProcessor.Average30Seconds, 0, MidpointRounding.AwayFromZero);
            Average1Minute = Double.Round(arduinoProcessor.Average1Minute, 0, MidpointRounding.AwayFromZero);
            Average30Minutes = Double.Round(arduinoProcessor.Average30Minutes, 0, MidpointRounding.AwayFromZero);

            Time = DateTime.Now.ToString("HH:mm:ss");
            WeatherData = WeatherUpdateThread.LatestWeatherData.CurrentData();
        }

        public bool Pump1Active { get; }

        public bool Pump2Active { get; }

        public int SensorValue { get; }

        public double Average30Seconds { get; }

        public double Average1Minute { get; }

        public double Average30Minutes { get; }

        public int RelayCount { get; }

        public string Time { get; }

        public CurrentWeatherData WeatherData { get; }
    }
}
