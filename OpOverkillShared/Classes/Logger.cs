namespace OpOverkillShared
{
    public sealed class Logger : PluginManager.Abstractions.ILogger
    {
        #region ILogger Methods

        public void AddToLog(in PluginManager.LogLevel logLevel, in string data)
        {
            AddToLog(logLevel, String.Empty, data);
        }

        public void AddToLog(in PluginManager.LogLevel logLevel, in Exception exception)
        {
            AddToLog(logLevel, String.Empty, exception, String.Empty);
        }

        public void AddToLog(in PluginManager.LogLevel logLevel, in Exception exception, string data)
        {
            AddToLog(logLevel, String.Empty, exception, data);
        }

        public void AddToLog(in PluginManager.LogLevel logLevel, in string moduleName, in string data)
        {
#if TRACE
            System.Diagnostics.Trace.WriteLine($"{logLevel} {data}");
#endif

            Shared.EventLog.Add($"{logLevel.ToString()}\t{data}");
        }

        public void AddToLog(in PluginManager.LogLevel logLevel, in string moduleName, in Exception exception)
        {
            AddToLog(logLevel, moduleName, exception, String.Empty);
        }

        public void AddToLog(in PluginManager.LogLevel logLevel, in string moduleName, in Exception exception, string data)
        {
#if TRACE
            System.Diagnostics.Trace.WriteLine($"{logLevel} {moduleName} {exception?.Message}\r\n{data}");
#endif

            Shared.EventLog.Add(exception, $"{moduleName} {data}");
        }

        #endregion ILogger Methods
    }
}
