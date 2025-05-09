﻿using System;

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
        private const int ResponseCodeNotFound = 404;

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
        [Route("/Device/UpdateWeather/{deviceId}/{temperature}/{humidity}/{rainSensor}/{isRaining}/{lightSensor}/{light}/")]
        public JsonResult WeatherStationUpdate(long deviceId, decimal temperature, decimal humidity, decimal rainSensor, int isRaining, int lightSensor, int light)
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
            model.IsRaining = isRaining != 0;
            model.RainSensor = rainSensor;
            model.DaySensor = lightSensor;
            model.DayTime = light != 0;

            _dataProvider.UpdateWeather(deviceId, model);

            return GenerateJsonSuccessResponse();
        }

        [HttpPost]
        [Route("/Device/UpdateWaterPump/{deviceId}/{value}/{average}/{temperature}/{pump1Active}/{pump2Active}")]
        public JsonResult UpdateWaterPump(long deviceId, int value, int average, double temperature, int pump1Active, int pump2Active)
        {
            if (!ValidDevice(deviceId))
                return InvalidDeviceResponse();

            WaterPumpModel model = new WaterPumpModel(deviceId)
            {
                Temperature = temperature,
                Value = value,
                Average = average,
                Pump1Active = pump1Active != 0,
                Pump2Active = pump2Active != 0,
            };

            _dataProvider.UpdateWaterPump(deviceId, model);

            return GenerateJsonSuccessResponse();
        }

        [HttpGet]
        [Route("/Device/GetCurrentTemperature/{deviceId}/")]
        public JsonResult GetCurrentTemperature(long deviceId)
        {
            if (!ValidDevice(deviceId))
                return InvalidDeviceResponse();

            
            TemperatureDataModel result = _dataProvider.GetLatestTemperature(DeviceType.WeatherStation);

            if (result == null)
                return GenerateJsonErrorResponse(ResponseCodeNotFound, "No Data");

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
