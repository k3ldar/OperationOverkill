using System;

namespace OpOverkillShared.Models
{
    public class WeatherStationModel
    {
        public WeatherStationModel(long deviceId, string location, string ipAddress)
        {
            ArgumentException.ThrowIfNullOrEmpty(location);
            ArgumentException.ThrowIfNullOrEmpty(ipAddress);

            DeviceId = deviceId;
            Location = location;
            IpAddress = ipAddress;
        }

        public long DeviceId { get; }

        public string Location { get; }

        public string IpAddress { get; }

        public decimal Temperature { get; set; }
        
        public decimal Humidity { get; set; }
        
        public bool IsRaining { get; set; }

        public decimal RainSensor { get; set; }
    }
}
