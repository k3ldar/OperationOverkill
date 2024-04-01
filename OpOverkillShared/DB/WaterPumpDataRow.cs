using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using SimpleDB;

namespace OpOverkillShared.DB
{
    [Table("WaterPump", "WaterPump", CompressionType.None, CachingStrategy.SlidingMemory, WriteStrategy.Forced)]
    internal class WaterPumpDataRow : TableRowDefinition
    {
        private long _deviceId;
        private int _sensorValue;
        private int _averageValue;
        private double _temperature;
        private bool _pump1Active;
        private bool _pump2Active;

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

        public int Value
        {
            get => _sensorValue;

            set
            {
                if (value == _sensorValue)
                    return;

                _sensorValue = value;
                Update();
            }
        }

        public int Average
        {
            get => _averageValue;

            set
            {
                if (value == _averageValue)
                    return;

                _averageValue = value;
                Update();
            }
        }

        public double Temperature
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

        public bool Pump1
        {
            get => _pump1Active;

            set
            {
                if (value == _pump1Active)
                    return;

                _pump1Active = value;
                Update();
            }
        }

        public bool Pump2
        {
            get => _pump2Active;

            set
            {
                if (value == _pump2Active)
                    return;

                _pump2Active = value;
                Update();
            }
        }
    }
}
