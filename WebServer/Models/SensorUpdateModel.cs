﻿using System;

using OpOverkillShared;
using OpOverkillShared.Abstractions;
using OpOverkillShared.Classes;
using OpOverkillShared.Models;

namespace OpOverkillWebServer.Models
{
    public sealed class SensorUpdateModel
    {
        public SensorUpdateModel(IOpOverkillDataProvider dataProvider)
        {
            if (dataProvider == null)
                throw new ArgumentNullException(nameof(dataProvider));

            WaterPumpModel waterPump = dataProvider.GetWaterPump();
            Pump1Active = waterPump.Pump1Active;
            Pump2Active = waterPump.Pump2Active;
            SensorValue = waterPump.Value;
            SensorAverage = waterPump.Average;
            SensorTemperature = Math.Round(waterPump.Temperature, 1);

            WeatherStationModel weatherStation = dataProvider.GetWeatherStation(-1);
            Temperature = weatherStation.Temperature;
            Humidity = weatherStation.Humidity;
            RainSensor = Convert.ToInt32(weatherStation.RainSensor);
            IsRaining = weatherStation.IsRaining;

            Time = DateTime.Now.ToString("HH:mm:ss");

            if (WeatherUpdateThread.LatestWeatherData != null)
                WeatherForcast = WeatherUpdateThread.LatestWeatherData.CurrentData();

            WeatherStationModel actualTemp = dataProvider.GetWeatherStation(-1);
            Temperature = Math.Round(actualTemp.Temperature, 1);
            Humidity = Math.Round(actualTemp.Humidity, 0);
            RainSensor = Convert.ToInt32(actualTemp.RainSensor);
            IsRaining = actualTemp.IsRaining;
        }

        public bool Pump1Active { get; }

        public bool Pump2Active { get; }

        public int SensorValue { get; }

        public int SensorAverage { get; }

        public double SensorTemperature { get; }

        public decimal Temperature { get; }

        public decimal Humidity { get; }

        public int RainSensor { get; }

        public bool IsRaining { get; }

        public string Time { get; }

        public CurrentWeatherData WeatherForcast { get; }
    }
}
