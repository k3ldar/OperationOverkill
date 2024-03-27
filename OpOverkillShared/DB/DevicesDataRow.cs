using SimpleDB;

namespace OpOverkillShared.DB
{
    [Table("Devices", CompressionType.None, CachingStrategy.SlidingMemory, WriteStrategy.Forced)]
    internal sealed class DevicesDataRow : TableRowDefinition
    {
        private string _name;
        private string _ipAddress;
        private string _location;
        private DeviceType _deviceType;

        public string Name
        {
            get => _name;

            set
            {
                if (value == _name)
                    return;

                _name = value;
                Update();
            }
        }

        public string IpAddress
        {
            get => _ipAddress;

            set
            {
                if (value == _ipAddress)
                    return;

                _ipAddress = value;
                Update();
            }
        }

        public string Location
        {
            get => _location;

            set
            {
                if (value == _location)
                    return;

                _location = value;
                Update();
            }
        }

        public DeviceType DeviceType
        {
            get => _deviceType;

            set
            {
                if (value == _deviceType)
                    return;

                _deviceType = value;
                Update();
            }
        }
    }
}
