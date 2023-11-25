namespace OpOverkillShared
{
    public interface IArduinoProcessor
    {
        bool Pump1Active { get; }

        bool Pump2Active { get; }

        int SensorValue { get; }

        double Average15Seconds { get; }

        double Average30Seconds { get; }

        int RelayCount { get; }

        event EventHandler Pump1ActiveChanged;

        event EventHandler Pump2ActiveChanged;

        event EventHandler SensorValueChanged;

        event EventHandler RelayValueChanged;

        void Connect();
    }
}
