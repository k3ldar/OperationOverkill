using OpOverkillShared.Abstractions;
using OpOverkillShared.Models;

using Shared.Classes;

using SimpleDB;

namespace OpOverkillShared.DB
{
    internal class OpOverkillDataProvider : ThreadManager, IOpOverkillDataProvider
    {
        private readonly ISimpleDBOperations<DevicesDataRow> _devices;
        private readonly ISimpleDBOperations<HourWeatherDataRow> _hourWeather;
        private readonly ISimpleDBOperations<MinuteWeatherDataRow> _minuteWeather;
        private readonly ISimpleDBOperations<DailyWeatherDataRow> _dailyWeather;
        private readonly ISimpleDBOperations<WaterPumpDataRow> _waterPump;
        private readonly Dictionary<long, WeatherStationModel> _weatherStations = new();
        private readonly Dictionary<long, WaterPumpModel> _waterPumps = new();

        private WeatherStationModel _latestWeatherStationData;
        private WaterPumpModel _latestWaterPumpData;

        public OpOverkillDataProvider(ISimpleDBOperations<DevicesDataRow> devices,
            ISimpleDBOperations<DailyWeatherDataRow> dailyWeather,
            ISimpleDBOperations<HourWeatherDataRow> hourWeather,
            ISimpleDBOperations<MinuteWeatherDataRow> minuteWeather,
            ISimpleDBOperations<WaterPumpDataRow> waterPump)
            : base(null, TimeSpan.FromMinutes(1))
        {
            _devices = devices ?? throw new ArgumentNullException(nameof(devices));
            _dailyWeather = dailyWeather ?? throw new ArgumentNullException(nameof(dailyWeather));
            _hourWeather = hourWeather ?? throw new ArgumentNullException(nameof(hourWeather));
            _minuteWeather = minuteWeather ?? throw new ArgumentNullException(nameof(minuteWeather));
            _waterPump = waterPump ?? throw new ArgumentNullException(nameof(waterPump));

            foreach (var device in _devices.Select())
            {
                if (device.DeviceType == DeviceType.WeatherStation)
                {
                    _weatherStations.Add(device.Id, new WeatherStationModel(device.Id, device.Location, device.IpAddress));
                    _latestWeatherStationData = new(device.Id, device.Location, device.IpAddress);
                }
                else if (device.DeviceType == DeviceType.WaterPump)
                {
                    _waterPumps.Add(device.Id, new WaterPumpModel(device.Id));
                }
            }

            ThreadManager.ThreadStart(this, "Weather Data Management", ThreadPriority.Normal, true);
        }

        #region ThreadManager

        protected override bool Run(object parameters)
        {
            ProcessHourlyWeatherData();
            ProcessDailyWeatherData();

            return !HasCancelled();
        }

        #endregion ThreadManager

        #region IOpOverkillDataProvider

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
            if (deviceId == -1)
                deviceId = _devices.Select().FirstOrDefault(d => d.DeviceType == DeviceType.WeatherStation)?.Id ?? -1;

            if (_weatherStations.TryGetValue(deviceId, out WeatherStationModel weatherStationModel))
                return weatherStationModel;

            return _latestWeatherStationData ?? new WeatherStationModel(-1, "Unknown", "Invalid");
        }

        public bool IsValidDevice(long deviceId)
        {
            return _devices.Select(deviceId) != null;
        }

        public void UpdateWeather(long deviceId, WeatherStationModel model)
        {
            _latestWeatherStationData = new WeatherStationModel(model.DeviceId, model.Location, model.IpAddress)
            {
                Humidity = model.Humidity,
                Temperature = model.Temperature,
                IsRaining = model.IsRaining,
                RainSensor = model.RainSensor,
            };

            MinuteWeatherDataRow minuteWeatherDataRow = new MinuteWeatherDataRow()
            {
                DeviceId = deviceId,
                Humidity = model.Humidity,
                Temperature = model.Temperature,
                RainSensor = model.RainSensor,
                IsRaining = model.IsRaining,
            };

            _minuteWeather.Insert(minuteWeatherDataRow);
        }

        public TemperatureDataModel GetLatestTemperature(DeviceType deviceType)
        {
            if (_latestWeatherStationData == null)
                return new TemperatureDataModel();

            TimeSpan age = DateTime.UtcNow - _latestWeatherStationData.Created;

            TemperatureDataModel result = new TemperatureDataModel()
            {
                Temperature = _latestWeatherStationData.Temperature,
                Humidity = _latestWeatherStationData.Humidity,
                AgeMilliSeconds = Convert.ToInt64(age.TotalMilliseconds),
            };

            return result;
        }

        public WaterPumpModel GetWaterPump()
        {
            if (_latestWaterPumpData != null)
                return _latestWaterPumpData;

            _ = _waterPumps.TryGetValue(_devices.Select().FirstOrDefault(d => d.DeviceType == DeviceType.WaterPump)?.Id ?? -1, out WaterPumpModel waterPump);
            return waterPump ?? new WaterPumpModel(-1);
        }

        public void UpdateWaterPump(long deviceId, WaterPumpModel model)
        {
            if (_latestWaterPumpData == null || model.Pump1Active != _latestWaterPumpData.Pump1Active || model.Pump2Active !=  _latestWaterPumpData.Pump2Active)
            {
                WaterPumpDataRow waterPumpDataRow = new WaterPumpDataRow()
                {
                    DeviceId = model.DeviceId,
                    Value = model.Value,
                    Average = model.Average,
                    Pump1 = model.Pump1Active,
                    Pump2 = model.Pump2Active,
                    Temperature = model.Temperature,
                };

                _waterPump.Insert(waterPumpDataRow);
            }

            _latestWaterPumpData = model;

            if (_waterPumps.TryGetValue(deviceId, out WaterPumpModel waterPump))
            {
                waterPump.Temperature = model.Temperature;
                waterPump.Value = model.Value;
                waterPump.Average = model.Average;
                waterPump.Pump1Active = model.Pump1Active;
                waterPump.Pump2Active = model.Pump2Active;
            }
        }

        #endregion IOpOverkillDataProvider

        #region Private Methods

        private void ProcessHourlyWeatherData()
        {
            DateTime validWeather = DateTime.UtcNow.Date.AddHours(DateTime.UtcNow.Hour);

            var hourData = _minuteWeather.Select(mw => mw.Created < validWeather);

            if (hourData.Count > 0)
            {
                var hourly = hourData.GroupBy(row => new
                {
                    row.DeviceId,
                    row.Created.Date,
                    row.Created.Hour,
                });

                foreach (var hour in hourly)
                {
                    var items = hour.ToList();
                    DateTime hourDate = hour.Key.Date.AddHours(hour.Key.Hour);

                    HourWeatherDataRow hourWeatherDataRow = new HourWeatherDataRow()
                    {
                        Temperature = items.Average(a => a.Temperature),
                        Humidity = items.Average(a => a.Humidity),
                        IsRaining = items.Exists(e => e.IsRaining),
                        RainSensor = items.Average(a => a.RainSensor),
                        Date = hourDate,
                    };

                    _hourWeather.Insert(hourWeatherDataRow);
                    _minuteWeather.Delete(items);
                }
            }
        }

        private void ProcessDailyWeatherData()
        {
            DateTime validWeather = DateTime.UtcNow.Date;
            DateTime existingDate = _dailyWeather.RecordCount == 0 ? DateTime.MinValue : _dailyWeather.Select().Max(a => a.Date.Date);

            var dailyData = _hourWeather.Select(mw => mw.Created.Date > existingDate && mw.Created.Date < validWeather);

            if (dailyData.Count > 0)
            {
                var daily = dailyData.GroupBy(row => new
                {
                    row.DeviceId,
                    row.Created.Date,
                });

                foreach (var day in daily)
                {
                    var items = day.ToList();
                    DateTime dailyDate = day.Key.Date;

                    DailyWeatherDataRow hourWeatherDataRow = new DailyWeatherDataRow()
                    {
                        Temperature = items.Average(a => a.Temperature),
                        Humidity = items.Average(a => a.Humidity),
                        IsRaining = items.Exists(e => e.IsRaining),
                        RainSensor = items.Average(a => a.RainSensor),
                        Date = dailyDate,
                    };

                    _dailyWeather.Insert(hourWeatherDataRow);
                }
            }
        }

        #endregion Private Methods
    }
}
