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
            _ = app.ApplicationServices.GetService<ISimpleDBOperations<DailyWeatherDataRow>>();
            _ = app.ApplicationServices.GetService<ISimpleDBOperations<HourWeatherDataRow>>();
            _ = app.ApplicationServices.GetService<ISimpleDBOperations<MinuteWeatherDataRow>>();
            _ = app.ApplicationServices.GetService<ISimpleDBOperations<WaterPumpDataRow>>();

            // start threads
            _ = app.ApplicationServices.GetService<IOpOverkillDataProvider>();
            _ = app.ApplicationServices.GetService<WeatherUpdateThread>();
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
            services.AddSingleton(typeof(TableRowDefinition), typeof(DailyWeatherDataRow));
            services.AddSingleton(typeof(TableRowDefinition), typeof(HourWeatherDataRow));
            services.AddSingleton(typeof(TableRowDefinition), typeof(MinuteWeatherDataRow));
            services.AddSingleton(typeof(TableRowDefinition), typeof(WaterPumpDataRow));
        }

        public void Configure(in IApplicationBuilder app)
        {
            // from interface but not used in this context
        }

        public void ConfigureServices(IServiceCollection services)
        {
            services.AddSingleton<ILogger, Logger>();
            services.AddSingleton<ISettingsProvider, DefaultSettingProvider>();
            services.AddTransient<ApiWrapper>();
            services.AddSingleton<IOpOverkillDataProvider, OpOverkillDataProvider>();
            services.AddSingleton<WeatherUpdateThread>();
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
