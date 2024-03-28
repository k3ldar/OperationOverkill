﻿using SimpleDB;

namespace OpOverkillShared.DB
{
    [Table("WeatherData", "Minute", CompressionType.None, CachingStrategy.SlidingMemory, WriteStrategy.Forced)]
    internal class MinuteWeatherDataRow : TableRowDefinition
    {
        private long _deviceId;
        private decimal _temperature;
        private decimal _humidity;
        private decimal _rainSensor;
        private bool _isRaining;

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
