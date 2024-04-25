// Please see documentation at https://docs.microsoft.com/aspnet/core/client-side/bundling-and-minification
// for details on configuring this project to bundle and minify static web assets.

// Write your JavaScript code.
let opOverkill = (function () {
    let _settings = {
        connState:'',
        socket: '',
        serverIp: '',
        updInterval: '',
        conInterval: '',
        sendUpdateRequest: false,
        isConnected: false,
        statusUpdateCode: '',
        connectedYes: 'Yes',
        connectedNo: 'No',
        machineHash: 0,
        useAjax: true,
    }

    let that = {
        init: function (settings) {
            _settings = settings;

            _settings.machineHash = -1;

            if (_settings.isConnected == undefined)
                _settings.isConnected = false;


            $(document).ready(function () {
                if (_settings.useAjax === true) {
                    window.setInterval(that.updateStatistics, 1000);
                    _settings.connState.style.visibility = "hidden";
                }
                else {
                    that.updInterval = setInterval(function () {
                        if (_settings.isConnected === false) {
                            that.connectToServer();
                        }
                        else if (_settings.sendUpdateRequest === true) {
                            _settings.sendUpdateRequest = false;
                            _settings.socket.send(_settings.statusUpdateCode);
                        }
                    }, 200);
                }
            });
        },

        updateStatistics: function () {
            $.ajax({
                type: 'GET',
                url: '/Home/GetSensorUpdates',
                cache: false,
                success: function (response) {
                    let wsd = JSON.parse(response.responseData);
                    that.updateWaterSensorStatus(wsd);
                },
            });

        },

        connected: function () {
            _settings.isConnected = true;
            _settings.connState.innerText = "Server Connected";
            _settings.connState.classList.remove("bg-danger");
            _settings.connState.classList.remove("bg-warning");
            _settings.connState.classList.add("bg-success");
        },

        disconnected: function () {
            _settings.isConnected = false;
            _settings.connState.innerText = "Server Not Connected";
            _settings.connState.classList.remove("bg-success");
            _settings.connState.classList.remove("bg-danger");
            _settings.connState.classList.add("bg-warning");
        },

        disable: function () {
            _settings.connState.classList.remove("bg-success");
            _settings.connState.classList.remove("bg-warning");
            _settings.connState.classList.add("bg-danger");
        },

        updateState: function () {
            if (!_settings.socket) {
                that.disable();
            } else {
                switch (_settings.socket.readyState) {
                    case WebSocket.CLOSED:
                        that.disconnected();
                        break;
                    case WebSocket.CLOSING:
                        that.disconnected();
                        break;
                    case WebSocket.CONNECTING:
                        that.disconnected();
                        break;
                    case WebSocket.OPEN:
                        that.connected();
                        break;
                    default:
                        that.disconnected();
                        break;
                }
            }
        },

        connectToServer: function () {
            _settings.socket = new WebSocket(_settings.serverIp);
            _settings.socket.onopen = function (event) {
                that.updateState();
                _settings.sendUpdateRequest = true;
            };
            _settings.socket.onclose = function (event) {
                that.updateState();
            };
            _settings.socket.onerror = that.updateState;
            _settings.socket.onmessage = function (event) {

                if (event.data != '') {
                    let jsonData = $.parseJSON(event.data);

                    
                    if (jsonData.success)
                    {
                        if (jsonData.request === 'WaterSensor') {
                            that.updateWaterSensorStatus(jsonData.message);
                        }
                    }
                }
                _settings.sendUpdateRequest = true;
            };
        },

        updateWaterSensorStatus: function (wsd) {
            // water sensor
            that.updateSpan(document.getElementById("spTime"), wsd.Time);
            that.updateSpan(document.getElementById("spPump1"), wsd.Pump1Active);
            that.updateSpan(document.getElementById("spPump2"), wsd.Pump2Active);
            that.updateSpan(document.getElementById("spSensor"), wsd.SensorValue);
            that.updateSpan(document.getElementById("spSensorAverage"), wsd.SensorAverage);
            that.updateSpan(document.getElementById("spSensorTemperature"), wsd.SensorTemperature);

            // weather station
            that.updateSpan(document.getElementById("spWeatherStationTemperature"), wsd.Temperature);
            that.updateSpan(document.getElementById("spWeatherStationHumidity"), wsd.Humidity);
            that.updateSpan(document.getElementById("spWeatherStationRainSensor"), wsd.RainSensor);
            that.updateSpan(document.getElementById("spWeatherStationIsRaining"), wsd.IsRaining);

            // weather forcast
            that.updateSpan(document.getElementById("spTemp"), wsd.WeatherForcast.Temperature_2m);
            that.updateSpan(document.getElementById("spHumid"), wsd.WeatherForcast.Relative_humidity_2m);
            that.updateSpan(document.getElementById("spDewPoint"), wsd.WeatherForcast.Dew_point_2m);
            that.updateSpan(document.getElementById("spPrecipProb"), wsd.WeatherForcast.Precipitation_probability);
            that.updateSpan(document.getElementById("spPrecip"), wsd.WeatherForcast.Precipitation);
            that.updateSpan(document.getElementById("spRain"), wsd.WeatherForcast.Rain);
            that.updateSpan(document.getElementById("spShowers"), wsd.WeatherForcast.Showers);
            that.updateSpan(document.getElementById("spWindSpeed"), wsd.WeatherForcast.WindSpeed);
            that.updateSpan(document.getElementById("spWindDirection"), wsd.WeatherForcast.WindDirection);
        },

        updateSpan: function (n, v) {
            n.innerText = v;
        },

        htmlEscape: function (str) {
            return str.toString()
                .replace(/&/g, '&amp;')
                .replace(/"/g, '&quot;')
                .replace(/'/g, '&#39;')
                .replace(/</g, '&lt;')
                .replace(/>/g, '&gt;');
        }
    };

    return that;
})();