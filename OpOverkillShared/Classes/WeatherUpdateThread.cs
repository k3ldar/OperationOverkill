﻿using Shared.Classes;

namespace OpOverkillShared.Classes
{
    public sealed class WeatherUpdateThread : ThreadManager
    {
        private static readonly Uri _uri = new("https://api.open-meteo.com/v1/forecast?latitude=55.25670612052087&longitude=10.667303387776974&hourly=temperature_2m,relative_humidity_2m,dew_point_2m,precipitation_probability,precipitation,rain,showers,wind_speed_10m,wind_direction_10m&timezone=GMT&forecast_days=1");
        private static readonly object _lockObject = new();
        private static OpenMeteoApiJson _latestJson;

        public WeatherUpdateThread(ApiWrapper apiWrapper)
            : base(apiWrapper, TimeSpan.FromMinutes(5))
        {
            ThreadManager.ThreadStart(this, "Weather Forcast", ThreadPriority.BelowNormal);
        }

        public static OpenMeteoApiJson LatestWeatherData
        {
            get
            {
                using (TimedLock tl = TimedLock.Lock(_lockObject))
                {
                    return _latestJson;
                }
            }
        }

        protected override bool Run(object parameters)
        {
            if (parameters is ApiWrapper apiWrapper)
            {
                OpenMeteoApiJson newWeatherData = apiWrapper.CallGetApi<OpenMeteoApiJson>(_uri);

                if (newWeatherData != null)
                {
                    using (TimedLock tl = TimedLock.Lock(_lockObject))
                    {
                        newWeatherData.LastUpdated = DateTime.UtcNow;
                        _latestJson = newWeatherData;
                    }
                }
            }

            return !base.HasCancelled();
        }
    }
}
