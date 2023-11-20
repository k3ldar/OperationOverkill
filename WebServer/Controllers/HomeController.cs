using System;

using Microsoft.AspNetCore.Mvc;

using OpOverkill.Models;

using OpOverkillShared;

using SharedPluginFeatures;

namespace OpOverkill.Controllers
{
    public class HomeController : BaseController
    {
        public const string Name = "Home";
        private readonly IArduinoProcessor _arduinoProcessor;

        public HomeController(IArduinoProcessor arduinoProcessor)
        {
            _arduinoProcessor = arduinoProcessor ?? throw new ArgumentNullException(nameof(arduinoProcessor));
        }

        [HttpGet]
        public IActionResult Index()
        {
            return View(CreateIndexModel());
        }

        private IndexViewModel CreateIndexModel()
        {
            return new IndexViewModel(GetModelData(), _arduinoProcessor);
        }
    }
}
