using System;

using Microsoft.AspNetCore.Mvc;

using OpOverkill.Models;

using OpOverkillShared;
using OpOverkillShared.Abstractions;

using OpOverkillWebServer.Models;

using SharedPluginFeatures;

namespace OpOverkill.Controllers
{
    public class HomeController : BaseController
    {
        public const string Name = "Home";
        private readonly IArduinoProcessor _arduinoProcessor;
        private readonly IOpOverkillDataProvider _dataProvider;

        public HomeController(IArduinoProcessor arduinoProcessor, IOpOverkillDataProvider dataProvider)
        {
            _arduinoProcessor = arduinoProcessor ?? throw new ArgumentNullException(nameof(arduinoProcessor));
            _dataProvider = dataProvider ?? throw new ArgumentNullException(nameof(dataProvider));
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
            return GenerateJsonSuccessResponse(new SensorUpdateModel(_arduinoProcessor, _dataProvider));
        }

        private IndexViewModel CreateIndexModel()
        {
            return new IndexViewModel(GetModelData(), _arduinoProcessor, _dataProvider);
        }
    }
}
