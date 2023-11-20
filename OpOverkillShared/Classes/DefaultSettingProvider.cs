using AppSettings;

using Microsoft.Extensions.Configuration;

using PluginManager.Abstractions;

namespace OpOverkillShared.Classes
{
    internal class DefaultSettingProvider : ISettingsProvider
    {
        #region Private Members

        private readonly string _rootPath;
        private readonly ISettingOverride _settingOverride;
        private readonly IApplicationOverride _appOverride;
        private readonly ISettingError _settingsError;

        #endregion Private Members

        #region Constructors

        public DefaultSettingProvider(ISettingOverride settingOverride, IApplicationOverride appOverride, ISettingError settingsError)
        {
            _settingOverride = settingOverride;
            _appOverride = appOverride;
            _settingsError = settingsError;
            _rootPath = OverkillHelper.GetSettingsFile();
        }

        #endregion Constructors

        #region ISettingsProvider Methods

        public T GetSettings<T>(in string storage, in string sectionName)
        {
            if (String.IsNullOrEmpty(storage))
                throw new ArgumentNullException(nameof(storage));

            if (String.IsNullOrEmpty(sectionName))
                throw new ArgumentNullException(nameof(sectionName));

            ConfigurationBuilder builder = new();
            IConfigurationBuilder configBuilder = builder.SetBasePath(Path.GetDirectoryName(_rootPath));
            configBuilder.AddJsonFile(storage);
            IConfigurationRoot config = builder.Build();
            T Result = (T)Activator.CreateInstance(typeof(T));
            config.GetSection(sectionName).Bind(Result);

            return ValidateSettings<T>.Validate(Result, _settingOverride, _settingsError, _appOverride);
        }

        public T GetSettings<T>(in string sectionName)
        {
            return GetSettings<T>(Constants.AppSettings, sectionName);
        }

        #endregion ISettingsProvider Methods
    }
}
