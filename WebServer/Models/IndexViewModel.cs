using System;

using OpOverkillShared;
using OpOverkillShared.Abstractions;
using OpOverkillShared.Models;

using SharedPluginFeatures;

namespace OpOverkill.Models
{
    public class IndexViewModel : BaseModel
    {
        public IndexViewModel(BaseModelData modelData, IOpOverkillDataProvider dataProvider)
            : base(modelData)
        {
            if (dataProvider == null)
                throw new ArgumentNullException(nameof(dataProvider));

            WaterPumpModel waterPumpModel = dataProvider.GetWaterPump();
            Pump1Active = waterPumpModel.Pump1Active;
            Pump2Active = waterPumpModel.Pump2Active;
            SensorValue = waterPumpModel.Value;
            SensorAverage = waterPumpModel.Average;
            TemperatureForcast = waterPumpModel.Temperature;

            WeatherStationModel actualTemp = dataProvider.GetWeatherStation(-1);
            Temperature = Math.Round(actualTemp.Temperature, 1);
            Humidity = Math.Round(actualTemp.Humidity, 0);
            RainSensor = Convert.ToInt32(actualTemp.RainSensor);
            IsRaining = actualTemp.IsRaining;
        }

        public bool Pump1Active { get; }

        public bool Pump2Active { get; }

        public int SensorValue { get; }

        public int SensorAverage { get; }

        public double TemperatureForcast { get; }

        public decimal Temperature { get; }

        public decimal Humidity { get; }

        public int RainSensor { get; }

        public bool IsRaining { get; }
    }
}
