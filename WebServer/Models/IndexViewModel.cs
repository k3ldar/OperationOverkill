using System;
using System.Collections.Generic;

using OpOverkillShared;
using OpOverkillShared.Abstractions;

using SharedPluginFeatures;

namespace OpOverkill.Models
{
    public class IndexViewModel : BaseModel
    {
        public IndexViewModel(BaseModelData modelData, IArduinoProcessor arduinoProcessor, IOpOverkillDataProvider dataProvider)
            : base(modelData)
        {
            if (arduinoProcessor == null)
                throw new ArgumentNullException(nameof(arduinoProcessor));

            if (dataProvider == null)
                throw new ArgumentNullException(nameof(dataProvider));

            Pump1Active = arduinoProcessor.Pump1Active;
            Pump2Active = arduinoProcessor.Pump2Active;
            SensorValue = arduinoProcessor.SensorValue;
            SensorAverage = arduinoProcessor.SensorAverage;
            TemperatureForcast = arduinoProcessor.TemperatureForcast;

            var actualTemp = dataProvider.GetLatestTemperature(DeviceType.WeatherStation);
            Temperature = Math.Round(actualTemp.Temperature, 1);
            Humidity = Math.Round(actualTemp.Humidity, 0);
        }

        public bool Pump1Active { get; }

        public bool Pump2Active { get; }

        public int SensorValue { get; }

        public int SensorAverage { get; }

        public double TemperatureForcast { get; }

        public decimal Temperature { get; }

        public decimal Humidity { get; }
    }
}
