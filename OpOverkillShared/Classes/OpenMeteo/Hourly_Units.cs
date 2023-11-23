namespace OpOverkillShared.Classes
{
    public sealed class Hourly_Units
    {
        public string time { get; set; }
        public string temperature_2m { get; set; }
        public string relative_humidity_2m { get; set; }
        public string dew_point_2m { get; set; }
        public string precipitation_probability { get; set; }
        public string precipitation { get; set; }
        public string rain { get; set; }
        public string showers { get; set; }

        public string wind_speed_10m { get; set; }

        public string wind_direction_10m { get; set; }
    }
}