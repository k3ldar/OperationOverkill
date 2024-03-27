using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.DependencyInjection;
using OpOverkillShared.Abstractions;
using OpOverkillShared.Classes;
using OpOverkillShared.DB;

using PluginManager.Abstractions;

using SharedPluginFeatures;

using SimpleDB;

namespace OpOverkillShared
{
    public sealed class PluginInitialisation : IPlugin, IInitialiseEvents
    {
        public void AfterConfigure(in IApplicationBuilder app)
        {
            _ = app.ApplicationServices.GetService<ISimpleDBOperations<DevicesDataRow>>();
        }

        public void AfterConfigureServices(in IServiceCollection services)
        {
            // from interface but not used in this context
        }

        public void BeforeConfigure(in IApplicationBuilder app)
        {
            // from interface but not used in this context
        }

        public void BeforeConfigureServices(in IServiceCollection services)
        {
            services.AddSingleton(typeof(TableRowDefinition), typeof(DevicesDataRow));
        }

        public void Configure(in IApplicationBuilder app)
        {
            // from interface but not used in this context
        }

        public void ConfigureServices(IServiceCollection services)
        {
            services.AddSingleton<ILogger, Logger>();
            services.AddSingleton<IArduinoProcessor, ArduinoProcessor>();
            services.AddTransient<IProcessorMediator, ProcessorMediator>();
            services.AddSingleton<WindowsComPort>();
            services.AddSingleton<ISettingsProvider, DefaultSettingProvider>();
            services.AddTransient<ApiWrapper>();
            services.AddSingleton<IOpOverkillDataProvider, OpOverkillDataProvider>();
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
