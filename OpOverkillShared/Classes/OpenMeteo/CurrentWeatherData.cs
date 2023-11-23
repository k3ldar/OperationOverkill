namespace OpOverkillShared.Classes
{
    public sealed class CurrentWeatherData
    {
        public DateTime Time { get; set; }
        public string Temperature_2m { get; set; }
        public string Relative_humidity_2m { get; set; }
        public string Dew_point_2m { get; set; }
        public string Precipitation_probability { get; set; }
        public string Precipitation { get; set; }
        public string Rain { get; set; }
        public string Showers { get; set; }
        public string WindSpeed { get; set; }
        public string WindDirection { get; set; }
    }
}
