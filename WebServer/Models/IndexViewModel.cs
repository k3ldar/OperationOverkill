using System;
using System.Collections.Generic;

using OpOverkillShared;

using SharedPluginFeatures;

namespace OpOverkill.Models
{
    public class IndexViewModel : BaseModel
    {
        public IndexViewModel(BaseModelData modelData, IArduinoProcessor arduinoProcessor)
            : base(modelData)
        {
            if (arduinoProcessor == null)
                throw new ArgumentNullException(nameof(arduinoProcessor));

            Pump1Active = arduinoProcessor.Pump1Active;
            Pump2Active = arduinoProcessor.Pump2Active;
            SensorValue = arduinoProcessor.SensorValue;
            SensorAverage = arduinoProcessor.SensorAverage;
        }

        public bool Pump1Active { get; }

        public bool Pump2Active { get; }

        public int SensorValue { get; }

        public int SensorAverage { get; }
    }
}
