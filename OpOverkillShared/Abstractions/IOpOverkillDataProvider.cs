using OpOverkillShared.Models;

namespace OpOverkillShared.Abstractions
{
    public interface IOpOverkillDataProvider
    {
        long RegisterDevice(string ipAddress, DeviceType deviceType);

        bool IsValidDevice(long deviceId);

        WeatherStationModel GetWeatherStation(long deviceId);

        void UpdateWeather(long deviceId, WeatherStationModel model);

        TemperatureDataModel GetLatestTemperature(DeviceType deviceType);

        WaterPumpModel GetWaterPump();

        void UpdateWaterPump(long deviceId, WaterPumpModel model);
    }
}