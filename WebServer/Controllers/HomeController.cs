using System;

using Microsoft.AspNetCore.Mvc;

using OpOverkill.Models;

using OpOverkillShared;

using OpOverkillWebServer.Models;

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

        [HttpGet]
        [AjaxOnly]
        public JsonResult GetSensorUpdates()
        {
            return GenerateJsonSuccessResponse(new SensorUpdateModel(_arduinoProcessor));
        }

        private IndexViewModel CreateIndexModel()
        {
            return new IndexViewModel(GetModelData(), _arduinoProcessor);
        }
    }
}
