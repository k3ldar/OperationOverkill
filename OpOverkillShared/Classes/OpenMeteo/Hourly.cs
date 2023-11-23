namespace OpOverkillShared.Classes
{
    public sealed class Hourly
    {
        public string[] time { get; set; }
        public double[] temperature_2m { get; set; }
        public double[] relative_humidity_2m { get; set; }
        public double[] dew_point_2m { get; set; }
        public double[] precipitation_probability { get; set; }
        public double[] precipitation { get; set; }
        public double[] rain { get; set; }
        public double[] showers { get; set; }

        public double[] wind_speed_10m { get; set; }

        public double[] wind_direction_10m { get; set; }
    }
}