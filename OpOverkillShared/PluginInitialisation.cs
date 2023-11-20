using Microsoft.Extensions.DependencyInjection;

using OpOverkillShared.Classes;

using PluginManager.Abstractions;

namespace OpOverkillShared
{
    public sealed class PluginInitialisation : IPlugin
    {
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddSingleton<ILogger, Logger>();
            services.AddSingleton<IArduinoProcessor, ArduinoProcessor>();
            services.AddTransient<IProcessorMediator, ProcessorMediator>();
            services.AddSingleton<WindowsComPort>();
            services.AddSingleton<ISettingsProvider, DefaultSettingProvider>();
        }

        public void Finalise()
        {
            // required by interface, not used in this context
        }

        public ushort GetVersion()
        {
            return 1;
        }

        public void Initialise(ILogger logger)
        {
            // required by interface, not used in this context
        }
    }
}
