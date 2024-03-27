using OpOverkillShared.Models;

namespace OpOverkillShared.Abstractions
{
    public interface IOpOverkillDataProvider
    {
        long RegisterDevice(string ipAddress, DeviceType deviceType);

        bool IsValidDevice(long deviceId);

        WeatherStationModel GetWeatherStation(long deviceId);
    }
}