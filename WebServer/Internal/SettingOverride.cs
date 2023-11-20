using AppSettings;

using OpOverkillShared.Classes;

namespace OpOverkill.Internal
{
    public class SettingOverride : ISettingOverride
    {
        public bool OverrideSettingValue(in string settingName, ref object propertyValue)
        {
            if (settingName.Equals("Path"))
            {
                propertyValue = OverkillHelper.GetDBPath();
                return true;
            }

            return false;
        }
    }
}
