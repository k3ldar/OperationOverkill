﻿using System;

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
            SensorAverage = arduinoProcessor.SensorAverage;

            Time = DateTime.Now.ToString("HH:mm:ss");
            WeatherData = WeatherUpdateThread.LatestWeatherData.CurrentData();
        }

        public bool Pump1Active { get; }

        public bool Pump2Active { get; }

        public int SensorValue { get; }

        public int SensorAverage { get; }

        public string Time { get; }

        public CurrentWeatherData WeatherData { get; }
    }
}
