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

        public decimal Temperature { get; set; } = -99;

        public decimal Humidity { get; set; } = -99;
        
        public bool IsRaining { get; set; }

        public decimal RainSensor { get; set; }

        public bool DayTime { get; set; }

        public DateTime Created { get; } = DateTime.UtcNow;
    }
}
