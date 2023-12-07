using System.Diagnostics;
using System.Net.WebSockets;
using System.Text;
using System.Text.Json;

using PluginManager.Abstractions;

using Shared.Classes;

namespace OpOverkillShared
{
    public class ProcessorMediator : IProcessorMediator
    {
        internal readonly static bool _isLicensed = true;
        private const int BufferSize = 8192;
        private readonly ILogger _logger;
        private readonly CancellationTokenSource _cancellationTokenSource;
        private WebSocket _webSocket;
        private ulong _messageId = 0;
        private string _clientId = null;
        private IArduinoProcessor _arduinoProcessor;

        public ProcessorMediator(IServiceProvider serviceProvider,
            ILogger logger)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _cancellationTokenSource = new CancellationTokenSource();
        }

        #region IProcessorMediator Methods

        public async Task ProcessClientCommunications(WebSocket webSocket, IArduinoProcessor arduinoProcessor, string clientId)
        {
            _webSocket = webSocket ?? throw new ArgumentNullException(nameof(webSocket));
            _arduinoProcessor = arduinoProcessor ?? throw new ArgumentNullException(nameof(arduinoProcessor));
            _clientId = clientId ?? "Unknown";

            byte[] receiveBuffer = new byte[1024];
            WebSocketReceiveResult receiveResult = await webSocket.ReceiveAsync(
                new ArraySegment<byte>(receiveBuffer), CancellationToken);

            AddEventsToProcessor(arduinoProcessor);
            try
            {
                while (!receiveResult.CloseStatus.HasValue)
                {

                    string request = Encoding.UTF8.GetString(receiveBuffer);
                    byte[] sendBuffer;

                    sendBuffer = ProcessRequest(request[..receiveResult.Count]);

                    await webSocket.SendAsync(
                        new ArraySegment<byte>(sendBuffer, 0, sendBuffer.Length),
                        receiveResult.MessageType,
                        receiveResult.EndOfMessage,
                        CancellationToken);

                    receiveResult = await webSocket.ReceiveAsync(
                        new ArraySegment<byte>(receiveBuffer), CancellationToken);
                }
            }
            catch (Exception ex)
            {
                Trace.WriteLine(String.Format("Error Closing Client Comms: {0}", ex.Message));
            }
            finally
            {
                _arduinoProcessor = null;
                RemoveEventsFromProcessor(arduinoProcessor);
            }

            if (receiveResult.CloseStatus != null || receiveResult.CloseStatusDescription != null)
            {
                await webSocket.CloseOutputAsync(
                    receiveResult.CloseStatus.Value,
                    receiveResult.CloseStatusDescription,
                    CancellationToken);
            }

            _cancellationTokenSource.Cancel();

            _webSocket = null;
        }

        #endregion IProcessorMediator Methods

        #region IProcessorMediator Properties

        public CancellationToken CancellationToken => _cancellationTokenSource.Token;

        #endregion IProcessorMediator Properties

        #region Processor Events

        private void AddEventsToProcessor(IArduinoProcessor arduinoProcessor)
        {
            arduinoProcessor.SensorValueChanged += ArduinoProcessor_SensorValueChanged;
            arduinoProcessor.SensorAverageValueChanged += ArduinoProcessor_SensorAverageValueChanged;
            arduinoProcessor.Pump1ActiveChanged += ArduinoProcessor_Pump1ActiveChanged;
            arduinoProcessor.Pump2ActiveChanged += ArduinoProcessor_Pump2ActiveChanged;
            arduinoProcessor.SensorTemperatureChanged += ArduinoProcessor_SensorTemperatureChanged;
        }

        private void RemoveEventsFromProcessor(IArduinoProcessor arduinoProcessor)
        {
            arduinoProcessor.SensorValueChanged -= ArduinoProcessor_SensorValueChanged;
            arduinoProcessor.SensorAverageValueChanged -= ArduinoProcessor_SensorAverageValueChanged;
            arduinoProcessor.Pump1ActiveChanged -= ArduinoProcessor_Pump1ActiveChanged;
            arduinoProcessor.Pump2ActiveChanged -= ArduinoProcessor_Pump2ActiveChanged;
        }

        private void ArduinoProcessor_Pump1ActiveChanged(object sender, EventArgs e)
        {
            _logger.AddToLog(PluginManager.LogLevel.Information, $"Pump 1 Active Changed: {_arduinoProcessor?.Pump1Active}");
            SendMessage(nameof(IArduinoProcessor.Pump1ActiveChanged));
        }

        private void ArduinoProcessor_Pump2ActiveChanged(object sender, EventArgs e)
        {
            _logger.AddToLog(PluginManager.LogLevel.Information, $"Pump 2 Active Changed: {_arduinoProcessor?.Pump2Active}");
            SendMessage(nameof(IArduinoProcessor.Pump2ActiveChanged));
        }

        private void ArduinoProcessor_SensorTemperatureChanged(object sender, EventArgs e)
        {
            _logger.AddToLog(PluginManager.LogLevel.Information, $"Temperature Changed: {_arduinoProcessor.Temperature}");
        }

        private void ArduinoProcessor_SensorAverageValueChanged(object sender, EventArgs e)
        {
            _logger.AddToLog(PluginManager.LogLevel.Information, $"Relay Count Changed: {_arduinoProcessor?.SensorAverage}");
            SendMessage(nameof(IArduinoProcessor.SensorAverage));
        }

        private void ArduinoProcessor_SensorValueChanged(object sender, EventArgs e)
        {
            _logger.AddToLog(PluginManager.LogLevel.Information, $"Sensor Value Changed: {_arduinoProcessor?.SensorValue}");
            SendMessage(nameof(IArduinoProcessor.SensorValueChanged));
        }

        #endregion Processor Events

        #region Private Methods

        private void SendMessage(string message)
        {
            if (_arduinoProcessor == null)
                return;

            ClientBaseMessage clientBaseMessage = new(message, _arduinoProcessor);
            clientBaseMessage.ServerCpuStatus = ThreadManager.CpuUsage;

            byte[] json = Encoding.UTF8.GetBytes(JsonSerializer.Serialize(clientBaseMessage, Constants.DefaultJsonSerializerOptions));

            _webSocket.SendAsync(new ArraySegment<byte>(json, 0, json.Length), WebSocketMessageType.Binary, true, CancellationToken).ConfigureAwait(false);
        }

        private byte[] ProcessRequest(string request)
        {
            if (_messageId > ulong.MaxValue - 50)
                _messageId = 0;

            ClientBaseMessage response = new()
            {
                request = request,
                ServerCpuStatus = ThreadManager.CpuUsage,
                Identifier = $"{_clientId}:{_messageId++}",
            };

            string[] parts = request.Split(":", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);

            switch (parts[0])
            {
                case "WaterSensor":

                    response.request = "WaterSensor";

                    if (_arduinoProcessor != null)
                    {
                        response.message = _arduinoProcessor;
                        response.success = true;
                    }
                    else
                    {
                        response.success = false;
                    }

                    break;

                default:
                    response.message = "Invalid Request";
                    break;

            }

            byte[] json = Encoding.UTF8.GetBytes(JsonSerializer.Serialize(response, Constants.DefaultJsonSerializerOptions));

            if (json.Length > BufferSize)
                throw new InvalidOperationException();

            return json;
        }

        #endregion Private Methods
    }
}
