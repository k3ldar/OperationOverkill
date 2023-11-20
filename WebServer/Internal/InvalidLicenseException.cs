using System;

namespace OpOverkill.Internal
{
    public class InvalidLicenseException : Exception
    {
        public InvalidLicenseException(string message)
            : base(message)
        {
        }
    }
}
