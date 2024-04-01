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
        private readonly IOpOverkillDataProvider _dataProvider;

        public HomeController(IOpOverkillDataProvider dataProvider)
        {
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
            return GenerateJsonSuccessResponse(new SensorUpdateModel(_dataProvider));
        }

        private IndexViewModel CreateIndexModel()
        {
            return new IndexViewModel(GetModelData(), _dataProvider);
        }
    }
}
