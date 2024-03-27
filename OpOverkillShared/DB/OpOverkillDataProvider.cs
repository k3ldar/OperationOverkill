using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpOverkillShared.Abstractions;
using OpOverkillShared.Models;
using SimpleDB;

namespace OpOverkillShared.DB
{
    internal class OpOverkillDataProvider : IOpOverkillDataProvider
    {
        private readonly ISimpleDBOperations<DevicesDataRow> _devices;
        private readonly Dictionary<long, WeatherStationModel> _weatherStations = new();

        public OpOverkillDataProvider(ISimpleDBOperations<DevicesDataRow> devices)
        {
            _devices = devices ?? throw new ArgumentNullException(nameof(devices));

            foreach (var device in _devices.Select().Where(d => d.DeviceType == DeviceType.WeatherStation))
            {
                _weatherStations.Add(device.Id, new WeatherStationModel(device.Id, device.Location, device.IpAddress));
            }
        }

        public long RegisterDevice(string ipAddress, DeviceType deviceType)
        {
            // locked to local network !
            if (ipAddress.Contains("192.168.8"))
            {
                DevicesDataRow device = _devices.Select().FirstOrDefault(d => d.IpAddress.Equals(ipAddress, StringComparison.OrdinalIgnoreCase) && d.DeviceType == deviceType);

                if (device == null)
                {
                    device = new()
                    {
                        IpAddress = ipAddress,
                        Location = "unknown",
                        DeviceType = deviceType,
                    };

                    _devices.Insert(device);
                }

                if (deviceType == DeviceType.WeatherStation && GetWeatherStation(device.Id) == null)
                {
                    _weatherStations.Add(device.Id, new WeatherStationModel(device.Id, device.Location, device.IpAddress));
                }

                return device.Id;
            }

            return -1;
        }

        public WeatherStationModel GetWeatherStation(long deviceId)
        {
            if (_weatherStations.TryGetValue(deviceId, out WeatherStationModel weatherStationModel))
                return weatherStationModel;

            return null;
        }

        public bool IsValidDevice(long deviceId)
        {
            return _devices.Select(deviceId) != null;
        }
    }
}
