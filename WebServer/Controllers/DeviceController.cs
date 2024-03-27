using System;

using Microsoft.AspNetCore.Mvc;

using OpOverkillShared;
using OpOverkillShared.Abstractions;
using OpOverkillShared.Models;

using OpOverkillWebServer.Models;

using SharedPluginFeatures;

namespace OpOverkillWebServer.Controllers
{
    public class DeviceController : BaseController
    {
        private const int ResponseCodeBadRequest = 400;
        private readonly IOpOverkillDataProvider _dataProvider;

        public DeviceController(IOpOverkillDataProvider dataProvider)
        {
            _dataProvider = dataProvider ?? throw new ArgumentNullException(nameof(dataProvider));
        }

        public IActionResult Index()
        {
            return View();
        }

        [HttpGet]
        [Route("/Device/RegisterDevice/{deviceType}/")]
        public JsonResult RegisterDevice(DeviceType deviceType)
        {
            long deviceId = _dataProvider.RegisterDevice(GetIpAddress(), deviceType);

            if (deviceId > -1)
                return GenerateJsonSuccessResponse(deviceId);

            return InvalidDeviceResponse();
        }

        [HttpPost]
        [Route("/Device/UpdateWeather/{deviceId}/{temperature}/{humidity}/{isRaining}/")]
        public JsonResult WeatherStationUpdate(long deviceId, decimal temperature, decimal humidity, bool isRaining)
        {
            if (!ValidDevice(deviceId))
                return InvalidDeviceResponse();

            WeatherStationModel model = _dataProvider.GetWeatherStation(deviceId);

            if (model == null)
            {
                return InvalidDeviceResponse();
            }

            model.Temperature = temperature;
            model.Humidity = humidity;
            model.IsRaining = isRaining;

            return GenerateJsonSuccessResponse();
        }

        [HttpGet]
        [Route("/Device/GetCurrentTemperature/{deviceId}/")]
        public JsonResult GetCurrentTemperature(long deviceId)
        {
            if (!ValidDevice(deviceId))
                return InvalidDeviceResponse();

            TemperatureDataModel result = new TemperatureDataModel()
            {
                Temperature = 12.3m,
                Humidity = 46.7m
            };

            return GenerateJsonSuccessResponse(result);
        }

        private JsonResult InvalidDeviceResponse()
        {
            return GenerateJsonErrorResponse(ResponseCodeBadRequest, "Invalid Device");
        }

        private bool ValidDevice(long deviceId)
        {
            return _dataProvider.IsValidDevice(deviceId);
        }
    }
}
