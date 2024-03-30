using System;

using OpOverkillShared;
using OpOverkillShared.Abstractions;
using OpOverkillShared.Classes;

namespace OpOverkillWebServer.Models
{
    public sealed class SensorUpdateModel
    {
        public SensorUpdateModel(IArduinoProcessor arduinoProcessor, IOpOverkillDataProvider dataProvider)
        {
            if (arduinoProcessor == null)
                throw new ArgumentNullException(nameof(arduinoProcessor));

            if (dataProvider == null)
                throw new ArgumentNullException(nameof(dataProvider));

            Pump1Active = arduinoProcessor.Pump1Active;
            Pump2Active = arduinoProcessor.Pump2Active;
            SensorValue = arduinoProcessor.SensorValue;
            SensorAverage = arduinoProcessor.SensorAverage;
            TemperatureForcast = Math.Round(arduinoProcessor.TemperatureForcast, 2);

            Time = DateTime.Now.ToString("HH:mm:ss");

            if (WeatherUpdateThread.LatestWeatherData != null)
                WeatherData = WeatherUpdateThread.LatestWeatherData.CurrentData();

            var actualTemp = dataProvider.GetLatestTemperature(DeviceType.WeatherStation);
            Temperature = Math.Round(actualTemp.Temperature, 1);
            Humidity = Math.Round(actualTemp.Humidity, 0);
        }

        public bool Pump1Active { get; }

        public bool Pump2Active { get; }

        public int SensorValue { get; }

        public int SensorAverage { get; }

        public double TemperatureForcast { get; }

        public decimal Temperature { get; }

        public decimal Humidity { get; }

        public string Time { get; }

        public CurrentWeatherData WeatherData { get; }
    }
}
