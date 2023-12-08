namespace OpOverkillShared.Classes
{
    public sealed class OpenMeteoApiJson
    {
        public double latitude { get; set; }
        public double longitude { get; set; }
        public double generationtime_ms { get; set; }
        public double utc_offset_seconds { get; set; }
        public string timezone { get; set; }
        public string timezone_abbreviation { get; set; }
        public double elevation { get; set; }
        public Hourly_Units hourly_units { get; set; }
        public Hourly hourly { get; set; }

        public DateTime LastUpdated { get; set; }

        public CurrentWeatherData CurrentData()
        {
            CurrentWeatherData result = new();

            int currentHour = DateTime.UtcNow.Hour;

            OpenMeteoApiJson currentData = WeatherUpdateThread.LatestWeatherData;

            if (currentData != null && currentHour < currentData.hourly.time.Length)
            {
                result.Time = DateTime.Parse(currentData.hourly.time[currentHour], null, System.Globalization.DateTimeStyles.RoundtripKind);
                result.Temperature_2m = $"{currentData.hourly.temperature_2m[currentHour]}{currentData.hourly_units.temperature_2m}";
                result.Relative_humidity_2m = $"{currentData.hourly.relative_humidity_2m[currentHour]}{currentData.hourly_units.relative_humidity_2m}";
                result.Dew_point_2m = $"{currentData.hourly.dew_point_2m[currentHour]}{currentData.hourly_units.dew_point_2m}";
                result.Precipitation_probability = $"{currentData.hourly.precipitation_probability[currentHour]}{currentData.hourly_units.precipitation_probability}";
                result.Precipitation = $"{currentData.hourly.precipitation[currentHour]}{currentData.hourly_units.precipitation}";
                result.Rain = $"{currentData.hourly.rain[currentHour]}{currentData.hourly_units.rain}";
                result.Showers = $"{currentData.hourly.showers[currentHour]}{currentData.hourly_units.showers}";
                result.WindSpeed = $"{currentData.hourly.wind_speed_10m[currentHour]}{currentData.hourly_units.wind_speed_10m}";
                result.WindDirection = $"{currentData.hourly.wind_direction_10m[currentHour]}{currentData.hourly_units.wind_direction_10m}";
            }

            return result;
        }
    }
}