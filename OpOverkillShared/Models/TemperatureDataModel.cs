﻿namespace OpOverkillShared.Models
{
    public sealed class TemperatureDataModel
    {
        public TemperatureDataModel()
        {
        }

        public decimal Temperature { get; set; }

        public decimal Humidity { get; set; }

        public long AgeMilliSeconds { get; set; }
    }
}
