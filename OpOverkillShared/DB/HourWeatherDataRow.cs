using SimpleDB;

namespace OpOverkillShared.DB
{
    [Table("WeatherData", "Hourly", CompressionType.None, CachingStrategy.SlidingMemory, WriteStrategy.Forced)]
    internal class HourWeatherDataRow : TableRowDefinition
    {
        private long _deviceId;
        private decimal _temperature;
        private decimal _humidity;
        private decimal _rainSensor;
        private bool _isRaining;
        private DateTime _date;

        [ForeignKey("Devices")]
        public long DeviceId
        {
            get => _deviceId;

            set
            {
                if (value == _deviceId)
                    return;

                _deviceId = value;
                Update();
            }
        }

        public DateTime Date
        {
            get => _date;

            set
            {
                if (value == _date)
                    return;

                _date = value;
                Update();
            }
        }

        public decimal Temperature
        {
            get => _temperature;

            set
            {
                if (value == _temperature)
                    return;

                _temperature = value;
                Update();
            }
        }

        public decimal Humidity
        {
            get => _humidity;

            set
            {
                if (value == _humidity)
                    return;

                _humidity = value;
                Update();
            }
        }

        public decimal RainSensor
        {
            get => _rainSensor;

            set
            {
                if (value == _rainSensor)
                    return;

                _rainSensor = value;
                Update();
            }
        }

        public bool IsRaining
        {
            get => _isRaining;

            set
            {
                if (value == _isRaining)
                    return;

                _isRaining = value;
                Update();
            }
        }
    }
}
