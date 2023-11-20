using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace OpOverkillShared.Classes
{
    public static class OverkillHelper
    {
        public static string GetRootPath()
        {
            return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        }

        public static string GetSettingsFile()
        {
            return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "OpOverkill", Constants.AppSettings);
        }

        public static string GetDBPath()
        {
            return Path.Combine(GetRootPath(), "DB");
        }

        public static string GetErrorPath()
        {
            return Path.Combine(GetRootPath(), "Errors");
        }
    }
}
