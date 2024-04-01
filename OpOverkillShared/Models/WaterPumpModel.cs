namespace OpOverkillShared.Models
{
    public sealed class WaterPumpModel
    {
        public WaterPumpModel(long deviceId)
        {
            DeviceId = deviceId;
        }

        public long DeviceId { get; }

        public int Value { get; set; }

        public int Average { get; set; }

        public double Temperature { get; set; }

        public bool Pump1Active { get; set; }

        public bool Pump2Active { get; set; }
    }
}
