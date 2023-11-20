using System;
using System.IO;
using System.Text.Json;

using OpOverkillShared.Classes;

using SharedPluginFeatures;

using static OpOverkillShared.Constants;

namespace OpOverkill.Internal
{
    public class ErrorManagerProvider : IErrorManager
    {
        public void ErrorRaised(in ErrorInformation errorInformation)
        {
            string path = OverkillHelper.GetErrorPath();

            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);

            string errText = JsonSerializer.Serialize<ErrorInformation>(errorInformation, OpOverkillShared.Constants.DefaultJsonSerializerOptions);

            string file = Path.Combine(path, $"{DateTime.UtcNow.Ticks}.error.txt");
            File.WriteAllText(file, errText);
        }

        public bool MissingPage(in string path, ref string replacePath)
        {
            if (path.Equals("/home", StringComparison.InvariantCultureIgnoreCase))
            {
                replacePath = "/";
                return true;
            }
            if (path.Equals("/home/index", StringComparison.InvariantCultureIgnoreCase))
            {
                replacePath = "/";
                return true;
            }

            return false;
        }
    }
}
