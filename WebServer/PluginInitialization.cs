using System;
using System.IO;

using AppSettings;

using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.DependencyInjection;

using OpOverkill.Internal;

using OpOverkillShared.DB;
using OpOverkillShared.Classes;

using PluginManager.Abstractions;

using SharedPluginFeatures;

using SimpleDB;

namespace OpOverkill
{
    public class PluginInitialization : IPlugin, IInitialiseEvents
    {
        #region IInitialiseEvents Methods

        public void AfterConfigure(in IApplicationBuilder app)
        {
            // not used in this context
        }

        public void AfterConfigureServices(in IServiceCollection services)
        {
            // not implemented at this time
        }

        public void BeforeConfigure(in IApplicationBuilder app)
        {
            // not implemented at this time
        }

        public void BeforeConfigureServices(in IServiceCollection services)
        {
            services.AddTransient<ISettingOverride, SettingOverride>();
            services.AddTransient<IApplicationOverride, ApplicationOverride>();
            services.AddTransient<ISettingError, SettingsError>();
            services.AddSingleton<IErrorManager, ErrorManagerProvider>();
            services.AddSingleton<ISharedPluginHelper, SharedPluginHelper>();
        }

        public void Configure(in IApplicationBuilder app)
        {
            // not implemented at this time
        }

        #endregion IInitialiseEvents Methods

        #region IPlugin Methods

        public void ConfigureServices(IServiceCollection services)
        {
            // not implemented at this time
        }

        public void Finalise()
        {
            // not implemented at this time
        }

        public ushort GetVersion()
        {
            return 1;
        }

        public void Initialise(ILogger logger)
        {
            Directory.CreateDirectory(OverkillHelper.GetErrorPath());
            Directory.CreateDirectory(OverkillHelper.GetDBPath());
        }

        #endregion IPlugin Methods
    }
}
