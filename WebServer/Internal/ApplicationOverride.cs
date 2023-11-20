using AppSettings;

namespace OpOverkill.Internal
{
    public class ApplicationOverride : IApplicationOverride
    {
        public bool ExpandApplicationVariable(string variableName, ref object value)
        {
            return false;
        }
    }
}
