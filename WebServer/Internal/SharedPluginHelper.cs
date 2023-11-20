using System;
using System.Collections.Generic;

using PluginManager.Abstractions;

using Shared.Classes;

using SharedPluginFeatures;

namespace OpOverkill.Internal
{
    public sealed class SharedPluginHelper : ISharedPluginHelper
    {
        #region Constants

        private const string MainMenuCache = "Main Menu Cache";

        #endregion Constants

        #region Private Methods

        private readonly IMemoryCache _memoryCache;
        private readonly IPluginClassesService _pluginClassesService;

        #endregion Private Methods

        #region Constructors

        public SharedPluginHelper(IMemoryCache memoryCache, IPluginClassesService pluginClassesService)
        {
            _memoryCache = memoryCache ?? throw new ArgumentNullException(nameof(memoryCache));
            _pluginClassesService = pluginClassesService ?? throw new ArgumentNullException(nameof(pluginClassesService));
        }

        #endregion Constructors

        public List<MainMenuItem> BuildMainMenu()
        {
            CacheItem cache = _memoryCache.GetExtendingCache().Get(MainMenuCache);

            if (cache == null)
            {
                cache = new CacheItem(MainMenuCache, _pluginClassesService.GetPluginClasses<MainMenuItem>());
                _memoryCache.GetExtendingCache().Add(MainMenuCache, cache);
            }

            return (List<MainMenuItem>)cache.Value;
        }
    }
}
