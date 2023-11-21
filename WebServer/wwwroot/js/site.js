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
            let p1 = document.getElementById("spPump1");
            let p2 = document.getElementById("spPump2");
            let sen = document.getElementById("spSensor");
            let rel = document.getElementById("spRelay");

            p1.innerText = wsd.Pump1Active;
            p2.innerText = wsd.Pump2Active;
            sen.innerText = wsd.SensorValue;
            rel.innerText = wsd.RelayCount;
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