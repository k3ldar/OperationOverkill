namespace OpOverkillWebServer.Models
{
    public sealed class TemperatureDataModel
    {
        public TemperatureDataModel()
        {
        }

        public decimal Temperature { get; set; }

        public decimal Humidity { get; set; }
    }
}
